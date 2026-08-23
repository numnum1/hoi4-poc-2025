#include "RtsWorldMap.h"

#include "BA_DataFunctions.h"
#include "Components/StaticMeshComponent.h"
#include "CountryName.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MapProjection.h"
#include "RHICommandList.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#if WITH_EDITOR
#include "TextureCompiler.h"
#endif
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "WorldMapSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsWorldMap, Log, All);

namespace
{
	/** Red picks the column, green the row, so the lookup textures are always this wide and a
	 * province's green channel decides how tall they have to be. The height is not free to choose:
	 * the map material derives the row from it, so a texture taller than the rows in use samples
	 * empty space instead of the province's color. */
	constexpr int32 LutWidth = 256;
	constexpr int32 MaxLutHeight = 256;

	/** Patches individual pixels of a lookup texture on the render thread.
	 *
	 * Reading the whole texture back before writing it looks wasteful, and is: the cost sits in the
	 * number of calls, not in the number of rows patched. It is unavoidable from here — a render
	 * target has no partial-update path that survives the next frame — which is why callers batch
	 * their changes into one call instead of writing province by province.
	 */
	template <typename ColorGetter>
	void WriteLutTexture(UCanvasRenderTarget2D* InRenderTarget, const TArray<FMapLutEntry>& InRows, ColorGetter InGetColor)
	{
		if (InRenderTarget == nullptr)
		{
			return;
		}

		TWeakObjectPtr<UCanvasRenderTarget2D> WeakRenderTarget = InRenderTarget;
		const int32 Width = InRenderTarget->SizeX;
		const int32 Height = InRenderTarget->SizeY;

		ENQUEUE_RENDER_COMMAND(WriteMapLut)(
			[WeakRenderTarget, InRows, InGetColor, Width, Height](FRHICommandListImmediate& RHICmdList)
			{
				if (!WeakRenderTarget.IsValid())
				{
					return;
				}

				FTextureRenderTargetResource* Resource = WeakRenderTarget->GetRenderTargetResource();
				if (Resource == nullptr)
				{
					return;
				}

				FTextureRHIRef TextureRHI = Resource->GetRenderTargetTexture();
				if (!TextureRHI)
				{
					return;
				}

				uint32 Stride = 0;
				uint8* TextureData = static_cast<uint8*>(RHICmdList.LockTexture2D(TextureRHI, 0, RLM_ReadOnly, Stride, false));
				if (TextureData == nullptr)
				{
					return;
				}

				TArray<FColor> Pixels;
				Pixels.SetNumUninitialized(Width * Height);
				for (int32 Y = 0; Y < Height; ++Y)
				{
					FMemory::Memcpy(Pixels.GetData() + Y * Width, TextureData + Y * Stride, Width * sizeof(FColor));
				}
				RHICmdList.UnlockTexture2D(TextureRHI, 0, false);

				for (const FMapLutEntry& Row : InRows)
				{
					const int32 Index = Row.Id.R + Row.Id.G * Width;
					if (Pixels.IsValidIndex(Index))
					{
						Pixels[Index] = FLinearColor(InGetColor(Row)).ToFColor(false);
					}
				}

				TextureData = static_cast<uint8*>(RHICmdList.LockTexture2D(TextureRHI, 0, RLM_WriteOnly, Stride, false));
				if (TextureData == nullptr)
				{
					return;
				}

				for (int32 Y = 0; Y < Height; ++Y)
				{
					FMemory::Memcpy(TextureData + Y * Stride, Pixels.GetData() + Y * Width, Width * sizeof(FColor));
				}
				RHICmdList.UnlockTexture2D(TextureRHI, 0, false);
			});
	}

	UCanvasRenderTarget2D* CreateLut(UObject* InOuter, const int32 InHeight)
	{
		UCanvasRenderTarget2D* Lut = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
			InOuter, UCanvasRenderTarget2D::StaticClass(), LutWidth, InHeight);
		if (Lut == nullptr)
		{
			return nullptr;
		}

		Lut->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
#if WITH_EDITOR
		Lut->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#else
		Lut->MipLoadOptions = ETextureMipLoadOptions::OnlyFirstMip;
#endif
		// Nearest and clamped throughout: these are lookup tables, and any filtering between two
		// neighbouring rows would blend two unrelated provinces' colors.
		Lut->SRGB = false;
		Lut->AddressX = TextureAddress::TA_Clamp;
		Lut->AddressY = TextureAddress::TA_Clamp;
		Lut->Filter = TextureFilter::TF_Nearest;
		Lut->MipsSamplerFilter = TextureFilter::TF_Nearest;
		Lut->UpdateResource();

		// A fresh render target's contents are undefined, and rows are only written once their
		// province is painted — without this, provinces would briefly sample whatever was in memory.
		UKismetRenderingLibrary::ClearRenderTarget2D(InOuter, Lut, FLinearColor::Black);
		return Lut;
	}
}

ARtsWorldMap::ARtsWorldMap()
{
	PrimaryActorTick.bCanEverTick = false;

	MapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MapMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GlobeMeshFinder(TEXT("/BloodAnvil/Art/Meshes/SM_Sphere.SM_Sphere"));
	if (GlobeMeshFinder.Succeeded())
	{
		GlobeMesh = GlobeMeshFinder.Object;
	}

	// Has to happen here, not only in ConfigureMesh: RebuildDynamicMaterials reads
	// MapMesh->GetNumMaterials(), which is 0 until a static mesh is assigned.
	MapMesh->SetStaticMesh(GlobeMesh);

	// The instance, not Mat_Map itself: the province view is selected by a static switch, and a
	// UMaterialInstanceDynamic created from the base material cannot override one — static switches
	// pick a shader permutation at compile time. Mat_Map alone renders the terrain without provinces.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MapMaterialFinder(TEXT("/BloodAnvil/Art/Materials/Instances/Mat_Map_Province.Mat_Map_Province"));
	if (MapMaterialFinder.Succeeded())
	{
		MapMaterials.Add(MapMaterialFinder.Object);
	}

	// A placeholder so the globe is textured out of the box; swap for the scenario's own province-ID
	// texture in the editor once one is authored.
	static ConstructorHelpers::FObjectFinder<UTexture2D> ProvinceIdTextureFinder(TEXT("/BloodAnvil/Demo/Data/Map/ProvID/T_ProvID_Game.T_ProvID_Game"));
	if (ProvinceIdTextureFinder.Succeeded())
	{
		ProvinceIdTexture = ProvinceIdTextureFinder.Object;
	}
}

void ARtsWorldMap::BeginPlay()
{
	Super::BeginPlay();

	ConfigureMesh();
	ConfigureSubsystem();
	EnsureLuts();
}

void ARtsWorldMap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Has to happen before RebuildDynamicMaterials: it reads MapMesh->GetNumMaterials(), which is 0
	// until a static mesh is assigned, so the dynamic materials would never reach any mesh slot.
	MapMesh->SetStaticMesh(GlobeMesh);
	RebuildDynamicMaterials();
	ConfigureMesh();
	ConfigureSubsystem();
	// Also outside Play so the globe already shows its province colors in the editor viewport.
	EnsureLuts();
}

void ARtsWorldMap::ConfigureSubsystem() const
{
	if (UWorldMapSubsystem* Subsystem = UWorldMapSubsystem::Get(this))
	{
		Subsystem->Configure(ProvinceMapData);
	}
}

void ARtsWorldMap::RebuildDynamicMaterials()
{
	// Rebuilt rather than appended to: every editor reconstruction would otherwise leave another
	// generation of dead dynamic materials behind, which the plugin's version of this did.
	MapDynamicMaterials.Empty();

	if (MapMaterials.IsEmpty())
	{
		UE_LOG(LogRtsWorldMap, Error, TEXT("%s has no MapMaterials assigned."), *GetName());
		return;
	}

	for (UMaterialInterface* Material : MapMaterials)
	{
		MapDynamicMaterials.Add(UMaterialInstanceDynamic::Create(Material, this));
	}

	const int32 SlotCount = FMath::Min(MapMesh->GetNumMaterials(), MapDynamicMaterials.Num());
	for (int32 Slot = 0; Slot < SlotCount; ++Slot)
	{
		if (MapDynamicMaterials[Slot])
		{
			MapMesh->SetMaterial(Slot, MapDynamicMaterials[Slot]);
		}
	}
}

void ARtsWorldMap::ConfigureMesh()
{
	MapMesh->SetStaticMesh(GlobeMesh);
	MapMesh->SetRelativeScale3D(FVector(MapRadius, MapRadius, MapRadius) / 100.0f);

	for (UMaterialInstanceDynamic* Material : MapDynamicMaterials)
	{
		if (Material)
		{
			Material->SetScalarParameterValue(FName("SphericalUVs"), 0.0f);
		}
	}
}

FVector2D ARtsWorldMap::VectorToUV(const FVector InImpactPoint) const
{
	return MapProjection::VectorToUV(InImpactPoint);
}

FVector ARtsWorldMap::UVToLocation(const FVector2D InUV) const
{
	return MapProjection::UVToLocation(InUV, MapRadius);
}

bool ARtsWorldMap::EnsureLuts()
{
	const UWorldMapSubsystem* Subsystem = UWorldMapSubsystem::Get(this);
	if (Subsystem == nullptr || !Subsystem->HasMapData() || ProvinceIdTexture == nullptr)
	{
		return false;
	}

	if (PrimaryLut == nullptr)
	{
		// Exactly the rows in use, matching the plugin's ceil(NumProvinces / 256): the material reads
		// the row off the texture's own height, so padding it out to 256 makes every province sample
		// an empty row instead of its color.
		const int32 HighestPixelRow = Subsystem->GetHighestPixelRow();
		const int32 Height = HighestPixelRow + 1;
		if (Height > MaxLutHeight)
		{
			UE_LOG(LogRtsWorldMap, Error,
			       TEXT("%s: province ids reach row %d, but a lookup texture holds at most %d."),
			       *GetName(), HighestPixelRow, MaxLutHeight - 1);
			return false;
		}

		PrimaryLut = CreateLut(this, Height);
		SecondaryLut = CreateLut(this, Height);
		NationalLut = CreateLut(this, Height);
		VfxLut = CreateLut(this, Height);

		if (PrimaryLut == nullptr || SecondaryLut == nullptr || NationalLut == nullptr || VfxLut == nullptr)
		{
			UE_LOG(LogRtsWorldMap, Error, TEXT("%s could not create its lookup textures."), *GetName());
			PrimaryLut = nullptr;
			return false;
		}
	}

	// Runs every call, not just when the LUTs are first created: RebuildDynamicMaterials replaces
	// MapDynamicMaterials with fresh instances on every construction, and a fresh dynamic material
	// starts out with none of its texture parameters set.
	for (UMaterialInstanceDynamic* Material : MapDynamicMaterials)
	{
		if (Material == nullptr)
		{
			continue;
		}

		Material->SetTextureParameterValue(FName("ProvID"), ProvinceIdTexture);
		Material->SetTextureParameterValue(FName("LUT - Primary"), PrimaryLut);
		Material->SetTextureParameterValue(FName("LUT - Secondary"), SecondaryLut);
		Material->SetTextureParameterValue(FName("LUT - National"), NationalLut);
		Material->SetTextureParameterValue(FName("LUT - VFX"), VfxLut);
	}

	return true;
}

bool ARtsWorldMap::ResolvePixel(const FHitResult& InHit, FMapPixelId& OutPixelId) const
{
	OutPixelId = FMapPixelId();

	if (!InHit.bBlockingHit)
	{
		return false;
	}

	const FVector2D UV = VectorToUV(InHit.ImpactPoint);

	FColor PixelColor;
	if (!ReadLookupPixel(UV, PixelColor))
	{
		return false;
	}

	const UWorldMapSubsystem* Subsystem = UWorldMapSubsystem::Get(this);
	OutPixelId = Subsystem ? Subsystem->DecodePixel(PixelColor) : FMapPixelId();

	UE_LOG(LogRtsWorldMap, Verbose,
	       TEXT("Hit on %s at %s -> UV (%f, %f) -> pixel (%d, %d)"),
	       *GetNameSafe(InHit.GetActor()), *InHit.ImpactPoint.ToString(), UV.X, UV.Y, OutPixelId.R, OutPixelId.G);

	return true;
}

void ARtsWorldMap::NotifyWorldClicked(const FHitResult& InHit)
{
	FMapPixelId PixelId;
	ResolvePixel(InHit, PixelId);

	// The miss is broadcast too, not swallowed: a click on nothing is a deselection.
	OnPixelClicked.Broadcast(PixelId);
}

void ARtsWorldMap::NotifyWorldHovered(const FHitResult& InHit)
{
	if (!bHoverHighlightEnabled)
	{
		return;
	}

	if (bHasHovered && InHit.bBlockingHit && InHit.ImpactPoint.Equals(LastHoverImpactPoint))
	{
		return;
	}

	FMapPixelId PixelId;
	ResolvePixel(InHit, PixelId);

	LastHoverImpactPoint = InHit.ImpactPoint;

	if (bHasHovered && PixelId == LastHoveredPixelId)
	{
		return;
	}

	bHasHovered = true;
	LastHoveredPixelId = PixelId;

	OnPixelHovered.Broadcast(PixelId);
}

void ARtsWorldMap::SetHighlightedPixel(const FMapPixelId InPixelId)
{
	if (bHasHighlight && HighlightedPixelId == InPixelId)
	{
		return;
	}

	const bool bHadPrevious = bHasHighlight;
	const FMapPixelId PreviousPixelId = HighlightedPixelId;

	HighlightedPixelId = InPixelId;
	bHasHighlight = true;

	RepaintHighlight(bHadPrevious, PreviousPixelId);
}

void ARtsWorldMap::ClearHighlightedPixel()
{
	if (!bHasHighlight)
	{
		return;
	}

	const FMapPixelId PreviousPixelId = HighlightedPixelId;

	HighlightedPixelId = FMapPixelId();
	bHasHighlight = false;

	RepaintHighlight(true, PreviousPixelId);
}

void ARtsWorldMap::RepaintHighlight(const bool bInHadPrevious, const FMapPixelId InPreviousPixelId)
{
	TArray<FMapLutEntry> ColorRows;
	TArray<FMapLutEntry> VfxRows;

	if (bInHadPrevious)
	{
		FMapLutEntry ClearedRow;
		ClearedRow.Id = InPreviousPixelId;
		VfxRows.Add(ClearedRow);

		// Restored from what was last painted, not from a color computed here: whoever painted it
		// last - the active map mode, or a modal's own overlay - is the one that gets to decide.
		if (const FMapLutEntry* PaintedRow = PaintedRows.Find(InPreviousPixelId))
		{
			ColorRows.Add(*PaintedRow);
		}
	}

	if (bHasHighlight)
	{
		FMapLutEntry HighlightRow;
		HighlightRow.Id = HighlightedPixelId;
		HighlightRow.Vfx = HoverVfxColor;
		VfxRows.Add(HighlightRow);

		if (const FMapLutEntry* PaintedRow = PaintedRows.Find(HighlightedPixelId))
		{
			ColorRows.Add(PaintedRow->Brightened(HoverHighlightStrength));
		}
	}

	PaintProvinceVfx(VfxRows);
	WriteColorRows(ColorRows);
}

void ARtsWorldMap::SetHoverHighlightEnabled(const bool bInEnabled)
{
	bHoverHighlightEnabled = bInEnabled;

	// Forgotten rather than kept: whatever was highlighted has already been painted away by the
	// caller, and remembering it would make the next enable skip the province the cursor is on.
	if (!bHoverHighlightEnabled)
	{
		bHasHovered = false;
		LastHoveredPixelId = FMapPixelId();
	}
}

bool ARtsWorldMap::ReadLookupPixel(const FVector2D InUV, FColor& OutColor) const
{
	if (ProvinceIdTexture == nullptr)
	{
		return false;
	}

#if WITH_EDITOR
	// Reading while the texture is still building would hand back the compile placeholder, whose mip
	// is far smaller than GetSizeX() claims - FindColor would index straight past the end of it.
	if (ProvinceIdTexture->IsDefaultTexture())
	{
		FTextureCompilingManager::Get().FinishCompilation({ ProvinceIdTexture.Get() });
	}
#endif

	const int32 Width = ProvinceIdTexture->GetSizeX();
	const int32 Height = ProvinceIdTexture->GetSizeY();

	const FTexturePlatformData* PlatformData = ProvinceIdTexture->GetPlatformData();
	const int64 PixelBytes = PlatformData != nullptr && PlatformData->Mips.Num() > 0
		? PlatformData->Mips[0].BulkData.GetBulkDataSize()
		: 0;
	if (Width <= 0 || Height <= 0 || PixelBytes < int64(Width) * Height * 4)
	{
		UE_LOG(LogRtsWorldMap, Error,
		       TEXT("%s: %s keeps %lld bytes of pixels, not the %lld that a %dx%d BGRA8 read needs."),
		       *GetName(), *GetNameSafe(ProvinceIdTexture), PixelBytes, int64(Width) * Height * 4, Width, Height);
		return false;
	}

	const int32 X = FMath::Clamp(FMath::FloorToInt(InUV.X * Width), 0, Width - 1);
	const int32 Y = FMath::Clamp(FMath::FloorToInt(InUV.Y * Height), 0, Height - 1);

	// Initialized rather than left to FindColor: it leaves its out parameter untouched when the lock
	// fails, which would hand back whatever was on the stack as a province color.
	OutColor = FColor::Black;
	UBA_DataFunctions::FindColor(ProvinceIdTexture.Get(), X, Y, OutColor);

	return true;
}

void ARtsWorldMap::PaintProvinces(const TArray<FMapLutEntry>& InRows)
{
	if (InRows.IsEmpty())
	{
		return;
	}

	if (!EnsureLuts())
	{
		if (!bPaintFailureLogged)
		{
			bPaintFailureLogged = true;
			UE_LOG(LogRtsWorldMap, Error, TEXT("%s cannot paint: its lookup textures are missing."), *GetName());
		}
		return;
	}

	for (const FMapLutEntry& Row : InRows)
	{
		PaintedRows.Add(Row.Id, Row);
	}

	// The caller sends the province's own color, not the highlight, so the highlighted row is
	// brightened on its way out - otherwise a map mode change would look like the cursor had left.
	TArray<FMapLutEntry> Rows = InRows;
	if (bHasHighlight)
	{
		for (FMapLutEntry& Row : Rows)
		{
			if (Row.Id == HighlightedPixelId)
			{
				Row = Row.Brightened(HoverHighlightStrength);
				break;
			}
		}
	}

	WriteColorRows(Rows);
}

void ARtsWorldMap::WriteColorRows(const TArray<FMapLutEntry>& InRows)
{
	if (InRows.IsEmpty() || !EnsureLuts())
	{
		return;
	}

	// Nothing is cleared first: the caller sends the provinces it wants changed, not the whole
	// world, and clearing would black out every province missing from this batch.
	WriteLutTexture(PrimaryLut, InRows, [](const FMapLutEntry& InRow) { return InRow.Primary; });
	WriteLutTexture(SecondaryLut, InRows, [](const FMapLutEntry& InRow) { return InRow.Secondary; });
	WriteLutTexture(NationalLut, InRows, [](const FMapLutEntry& InRow) { return InRow.National; });
}

void ARtsWorldMap::PaintProvinceVfx(const TArray<FMapLutEntry>& InRows)
{
	if (InRows.IsEmpty())
	{
		return;
	}

	if (!EnsureLuts())
	{
		if (!bPaintFailureLogged)
		{
			bPaintFailureLogged = true;
			UE_LOG(LogRtsWorldMap, Error, TEXT("%s cannot paint: its lookup textures are missing."), *GetName());
		}
		return;
	}

	WriteLutTexture(VfxLut, InRows, [](const FMapLutEntry& InRow) { return InRow.Vfx; });
}

void ARtsWorldMap::SetProvinceOwners(const TMap<FMapPixelId, FNationId>& InOwners)
{
	PixelOwnerNationId = InOwners;
}

FNationId ARtsWorldMap::FindOwnerNationIdForProvinceColor(const FColor& InProvinceColor) const
{
	const UWorldMapSubsystem* Subsystem = UWorldMapSubsystem::Get(this);
	if (Subsystem == nullptr)
	{
		return FNationId();
	}

	const FNationId* NationId = PixelOwnerNationId.Find(Subsystem->DecodePixel(InProvinceColor));
	return NationId ? *NationId : FNationId();
}

void ARtsWorldMap::SpawnCountryLabels(const TArray<FWorldMapLabel>& InLabels)
{
	for (const FWorldMapLabel& Label : InLabels)
	{
		ACountryName* Spawned = SpawnMapName();
		if (Spawned == nullptr)
		{
			UE_LOG(LogRtsWorldMap, Warning, TEXT("%s could not spawn a country name."), *GetName());
			return;
		}

		TArray<FVector> Points;
		Points.Reserve(Label.UVPoints.Num());
		for (const FVector2D& UVPoint : Label.UVPoints)
		{
			Points.Add(UVToLocation(UVPoint));
		}

		Spawned->Initialize(Points, Label.Name, this, Label.OwnerTags);
	}
}

ARtsWorldMap* ARtsWorldMap::GetRtsWorldMap(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (World == nullptr)
	{
		return nullptr;
	}

	return Cast<ARtsWorldMap>(UGameplayStatics::GetActorOfClass(World, ARtsWorldMap::StaticClass()));
}
