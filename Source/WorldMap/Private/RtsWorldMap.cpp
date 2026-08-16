#include "RtsWorldMap.h"

#include "BA_DataFunctions.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RHICommandList.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsWorldMap, Log, All);

namespace
{
	/** Red picks the column, green the row, so the lookup textures are always this wide and a
	 * province's green channel decides how tall they have to be. The height is not free to choose:
	 * the map material derives the row from it, so a texture taller than the rows in use samples
	 * empty space instead of the province's color. */
	constexpr int32 LutWidth = 256;
	constexpr int32 MaxLutHeight = 256;

	int32 PackPixelId(const uint8 InRed, const uint8 InGreen)
	{
		return (static_cast<int32>(InRed) << 8) | static_cast<int32>(InGreen);
	}

	/** Patches individual pixels of a lookup texture on the render thread.
	 *
	 * Reading the whole texture back before writing it looks wasteful, and is: the cost sits in the
	 * number of calls, not in the number of rows patched. It is unavoidable from here — a render
	 * target has no partial-update path that survives the next frame — which is why callers batch
	 * their changes into one call instead of writing province by province.
	 */
	template<typename ColorGetter>
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
	// Also outside Play so the globe already shows its province colors in the editor viewport.
	EnsureLuts();
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
	const FVector Normal = InImpactPoint.GetSafeNormal();
	return FVector2D(
		FMath::Atan2(Normal.X, Normal.Y) / (2.0f * PI) + 0.5f,
		FMath::Asin(-Normal.Z) / PI + 0.5f);
}

bool ARtsWorldMap::EnsureLuts()
{
	EnsureMapDataLookup();
	if (MapIdToPixelId.IsEmpty() || ProvinceIdTexture == nullptr)
	{
		return false;
	}

	if (PrimaryLut == nullptr)
	{
		// Exactly the rows in use, matching the plugin's ceil(NumProvinces / 256): the material reads
		// the row off the texture's own height, so padding it out to 256 makes every province sample
		// an empty row instead of its color.
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

void ARtsWorldMap::EnsureMapDataLookup()
{
	if (!MapIdToPixelId.IsEmpty())
	{
		return;
	}

	if (ProvinceMapData == nullptr)
	{
		if (!bLookupFailureLogged)
		{
			bLookupFailureLogged = true;
			UE_LOG(LogRtsWorldMap, Error,
				TEXT("%s has no ProvinceMapData assigned, so no province can be resolved or painted."),
				*GetName());
		}
		return;
	}

	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData == nullptr)
		{
			continue;
		}

		FMapPixelId PixelId;
		PixelId.R = MapData->Color.R;
		PixelId.G = MapData->Color.G;

		// Keyed on the row name, not the row's own Name field: that field holds the map editor's
		// label ("Arnhem"), while a province's MapId is the row key ("1893").
		MapIdToPixelId.Add(Row.Key, PixelId);
		PixelIdToMapId.Add(PackPixelId(PixelId.R, PixelId.G), Row.Key);
		HighestPixelRow = FMath::Max(HighestPixelRow, static_cast<int32>(PixelId.G));
	}
}

void ARtsWorldMap::EnsureProvinceLookup()
{
	EnsureMapDataLookup();

	if (!ProvinceIdToPixelId.IsEmpty() || MapIdToPixelId.IsEmpty())
	{
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr)
	{
		return;
	}

	for (const TPair<FName, FMapPixelId>& Entry : MapIdToPixelId)
	{
		const FProvinceId ProvinceId = Provinces->FindByMapId(Entry.Key);
		if (ProvinceId.IsValid())
		{
			ProvinceIdToPixelId.Add(ProvinceId, Entry.Value);
		}
	}

	// A table whose names match no province leaves the map unpainted with nothing else to show for
	// it, so the mismatch is called out rather than left to look like a rendering problem.
	if (ProvinceIdToPixelId.IsEmpty())
	{
		UE_LOG(LogRtsWorldMap, Error,
			TEXT("%s: none of the %d names in ProvinceMapData match a province MapId of the loaded scenario."),
			*GetName(), MapIdToPixelId.Num());
	}
}

FProvinceId ARtsWorldMap::ResolveProvinceAt(const FVector2D InUV)
{
	EnsureMapDataLookup();

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (ProvinceIdTexture == nullptr || Provinces == nullptr || PixelIdToMapId.IsEmpty())
	{
		return FProvinceId();
	}

	const int32 Width = ProvinceIdTexture->GetSizeX();
	const int32 Height = ProvinceIdTexture->GetSizeY();
	const int32 X = FMath::Clamp(FMath::FloorToInt(InUV.X * Width), 0, Width - 1);
	const int32 Y = FMath::Clamp(FMath::FloorToInt(InUV.Y * Height), 0, Height - 1);

	FColor PixelColor;
	UBA_DataFunctions::FindColor(ProvinceIdTexture, X, Y, PixelColor);

	const FName* MapId = PixelIdToMapId.Find(PackPixelId(PixelColor.R, PixelColor.G));
	return MapId ? Provinces->FindByMapId(*MapId) : FProvinceId();
}

void ARtsWorldMap::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	EnsureProvinceLookup();

	const bool bHasLuts = EnsureLuts();
	if (!bHasLuts || ProvinceIdToPixelId.IsEmpty())
	{
		if (!bPaintFailureLogged)
		{
			bPaintFailureLogged = true;
			UE_LOG(LogRtsWorldMap, Error,
				TEXT("%s cannot paint: lookup textures %s, province lookup holds %d entries."),
				*GetName(), bHasLuts ? TEXT("ready") : TEXT("missing"), ProvinceIdToPixelId.Num());
		}
		return;
	}

	TArray<FMapLutEntry> Rows;
	Rows.Reserve(InVisuals.Num());

	for (const TPair<FProvinceId, FProvinceVisuals>& Entry : InVisuals)
	{
		const FMapPixelId* PixelId = ProvinceIdToPixelId.Find(Entry.Key);
		if (PixelId == nullptr)
		{
			continue;
		}

		FMapLutEntry Row;
		Row.Id = *PixelId;
		Row.Primary = Entry.Value.PrimaryColor;
		Row.Secondary = Entry.Value.GetEffectiveSecondaryColor();
		Row.National = Entry.Value.GetEffectiveNationalColor();
		Rows.Add(Row);
	}

	if (Rows.IsEmpty())
	{
		if (!bPaintFailureLogged && !InVisuals.IsEmpty())
		{
			bPaintFailureLogged = true;
			UE_LOG(LogRtsWorldMap, Error,
				TEXT("%s: none of the %d provinces handed in are in the province-ID table."),
				*GetName(), InVisuals.Num());
		}
		return;
	}

	// Nothing is cleared first: the caller sends the provinces it wants changed, not the whole
	// world, and clearing would black out every province missing from this batch.
	WriteLutTexture(PrimaryLut, Rows, [](const FMapLutEntry& InRow) { return InRow.Primary; });
	WriteLutTexture(SecondaryLut, Rows, [](const FMapLutEntry& InRow) { return InRow.Secondary; });
	WriteLutTexture(NationalLut, Rows, [](const FMapLutEntry& InRow) { return InRow.National; });
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
