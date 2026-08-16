#include "RtsWorldMap.h"

#include "AssetTypeCategories.h"
#include "BA_DataFunctions.h"
#include "Components/StaticMeshComponent.h"
#include "CountryName.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PixelFormat.h"
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
#include "Provinces.h"
#include "RtsGameStateComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

	static ConstructorHelpers::FObjectFinder<UTexture2D> ProvinceIdLookupTextureFinder(TEXT("/BloodAnvil/Demo/Data/Map/ProvID/T_ProvID_Game_CPU.T_ProvID_Game_CPU"));
	if (ProvinceIdLookupTextureFinder.Succeeded())
	{
		ProvinceIdLookupTexture = ProvinceIdLookupTextureFinder.Object;
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

FVector ARtsWorldMap::UVToLocation(const FVector2D InUV) const
{
	const float Theta = InUV.X * 2.0f * PI;
	const float Phi = InUV.Y * PI;
	return FVector(
		-MapRadius * FMath::Sin(Phi) * FMath::Sin(Theta),
		-MapRadius * FMath::Sin(Phi) * FMath::Cos(Theta),
		MapRadius * FMath::Cos(Phi));
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

void ARtsWorldMap::EnsureMapDataLookup() const
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
		HighestPixelRow = FMath::Max(HighestPixelRow, static_cast<int32>(PixelId.G));
	}
}

void ARtsWorldMap::EnsureProvinceLookup() const
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
			PixelIdToProvinceId.Add(Entry.Value, ProvinceId);
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
	FColor PixelColor;
	if (!ReadLookupPixel(InUV, PixelColor))
	{
		return FProvinceId();
	}

	return FindProvinceByColor(PixelColor);
}

FProvinceId ARtsWorldMap::FindProvinceByColor(const FColor& InProvinceColor) const
{
	EnsureProvinceLookup();

	// Only red and green carry the id. A pixel read off the texture brings a blue and an alpha the
	// table never declares, so those two channels are dropped rather than compared.
	FMapPixelId PixelId;
	PixelId.R = InProvinceColor.R;
	PixelId.G = InProvinceColor.G;

	const FProvinceId* ProvinceId = PixelIdToProvinceId.Find(PixelId);
	return ProvinceId ? *ProvinceId : FProvinceId();
}

bool ARtsWorldMap::ReadLookupPixel(const FVector2D InUV, FColor& OutColor) const
{
	if (ProvinceIdLookupTexture == nullptr)
	{
		LogLookupTextureUnreadable(TEXT("is not assigned"));
		return false;
	}

	// A texture still being built hands out a 4x4 DXT1 placeholder rather than its real pixels, which
	// would otherwise be reported as the wrong compression settings and send whoever reads that
	// message off to change settings that were already correct.
	if (ProvinceIdLookupTexture->IsDefaultTexture())
	{
#if WITH_EDITOR
		FTextureCompilingManager::Get().FinishCompilation({ ProvinceIdLookupTexture.Get() });
#endif
		if (ProvinceIdLookupTexture->IsDefaultTexture())
		{
			LogLookupTextureUnreadable(TEXT("is still showing its placeholder — the texture never finished building"));
			return false;
		}
	}

	FTexturePlatformData* PlatformData = ProvinceIdLookupTexture->GetPlatformData();
	if (PlatformData == nullptr || PlatformData->Mips.Num() == 0)
	{
		LogLookupTextureUnreadable(TEXT("has no platform data"));
		return false;
	}

	// The pixels are indexed as FColor, so anything but BGRA8 would read neighbouring channels as a
	// province id and silently resolve to the wrong province rather than to none.
	if (PlatformData->PixelFormat != PF_B8G8R8A8)
	{
		LogLookupTextureUnreadable(*FString::Printf(
			TEXT("is %s at %dx%d, not uncompressed BGRA8 — it needs Compression Settings VectorDisplacementmap (RGBA8) and a rebuild"),
			GetPixelFormatString(PlatformData->PixelFormat),
			PlatformData->Mips[0].SizeX, PlatformData->Mips[0].SizeY));
		return false;
	}

	FTexture2DMipMap& Mip = PlatformData->Mips[0];
	const void* Data = Mip.BulkData.LockReadOnly();
	if (Data == nullptr)
	{
		// Unlocked even though there is nothing to read: the lock is taken either way, and leaving it
		// held would make every later read fail too.
		Mip.BulkData.Unlock();

		// The usual cause is a texture that streams its mips: the bulk data is then owned by the
		// streamer and simply not here to read.
		LogLookupTextureUnreadable(TEXT("keeps no CPU copy of its pixels — tick Never Stream and turn mipmaps off"));
		return false;
	}

	const int32 Width = Mip.SizeX;
	const int32 Height = Mip.SizeY;
	const int32 X = FMath::Clamp(FMath::FloorToInt(InUV.X * Width), 0, Width - 1);
	const int32 Y = FMath::Clamp(FMath::FloorToInt(InUV.Y * Height), 0, Height - 1);

	OutColor = static_cast<const FColor*>(Data)[Y * Width + X];
	Mip.BulkData.Unlock();

	return true;
}

void ARtsWorldMap::LogLookupTextureUnreadable(const TCHAR* InReason) const
{
	if (bLookupTextureFailureLogged)
	{
		return;
	}

	bLookupTextureFailureLogged = true;
	UE_LOG(LogRtsWorldMap, Error,
	       TEXT("%s: ProvinceIdLookupTexture (%s) %s, so no click can be resolved to a province."),
	       *GetName(), *GetNameSafe(ProvinceIdLookupTexture), InReason);
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

void ARtsWorldMap::SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData)
{
	NationNames = InNationData;
	GenerateCountryNames();
}

void ARtsWorldMap::EnsureAdjacencyLookup()
{
	EnsureProvinceLookup();

	if (!ProvinceAdjacencies.IsEmpty() || ProvinceIdToPixelId.IsEmpty() || ProvinceMapData == nullptr)
	{
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr)
	{
		return;
	}

	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData == nullptr)
		{
			continue;
		}

		const FProvinceId ProvinceId = Provinces->FindByMapId(Row.Key);
		if (!ProvinceId.IsValid())
		{
			continue;
		}

		TArray<FProvinceId>& Neighbours = ProvinceAdjacencies.Add(ProvinceId);
		for (const FName& AdjacentMapId : MapData->Adjacencies.Adjacency)
		{
			const FProvinceId AdjacentId = Provinces->FindByMapId(AdjacentMapId);
			if (AdjacentId.IsValid())
			{
				Neighbours.Add(AdjacentId);
				continue;
			}

			// Recorded rather than silently dropped: the map table covers the whole world while a
			// scenario loads a corner of it, so most neighbours resolve to nothing — and a province
			// whose neighbour the scenario does not know is exactly a province on the outer edge,
			// which is what the zone border test is looking for.
			ProvincesAdjacentToUnknown.Add(ProvinceId);
		}
	}
}

void ARtsWorldMap::EnsureUnitPositionLookup() const
{
	if (!ProvinceUnitPositions.IsEmpty() || ProvinceMapData == nullptr)
	{
		return;
	}

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr)
	{
		return;
	}

	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData == nullptr)
		{
			continue;
		}

		const FProvinceId ProvinceId = Provinces->FindByMapId(Row.Key);
		if (ProvinceId.IsValid())
		{
			ProvinceUnitPositions.Add(ProvinceId, MapData->UnitPosition);
		}
	}
}

bool ARtsWorldMap::FindUnitPositonOfProvince(const FProvinceId& ProvinceId, FVector2D& OutUnitPosition) const
{
	EnsureUnitPositionLookup();

	const FVector2D* Position = ProvinceUnitPositions.Find(ProvinceId);
	if (Position == nullptr)
	{
		return false;
	}

	OutUnitPosition = *Position;
	return true;
}

void ARtsWorldMap::EnsureNationOwnershipLookup() const
{
	if (!ProvinceOwnerNationId.IsEmpty() || NationNames.IsEmpty())
	{
		return;
	}

	for (const TPair<FNationId, FWorldMapNationData>& Entry : NationNames)
	{
		for (const FProvinceId& OwnedProvince : Entry.Value.OwnedProvinces)
		{
			ProvinceOwnerNationId.Add(OwnedProvince, Entry.Key);
		}
	}
}

FNationId ARtsWorldMap::FindOwnerNationIdForProvinceColor(const FColor& InProvinceColor) const
{
	const FProvinceId ProvinceId = FindProvinceByColor(InProvinceColor);
	if (!ProvinceId.IsValid())
	{
		return FNationId();
	}

	EnsureNationOwnershipLookup();

	const FNationId* NationId = ProvinceOwnerNationId.Find(ProvinceId);
	return NationId ? *NationId : FNationId();
}

bool ARtsWorldMap::GenerateMapNameZones(const TArray<FProvinceId>& InProvinces, TArray<FMapNameZone>& OutZones)
{
	EnsureAdjacencyLookup();
	if (ProvinceAdjacencies.IsEmpty())
	{
		return false;
	}

	TMap<FProvinceId, int32> ProvinceToZone;

	// Not TSet<FProvinceId>(InProvinces): that constructor form trips a known MSVC bug, see the note
	// atop Containers/Set.h.
	TSet<FProvinceId> ProvinceSet = TSet<FProvinceId>(TArray<FProvinceId>(InProvinces));

	for (int32 ZoneIndex = 0; ZoneIndex < OutZones.Num(); ++ZoneIndex)
	{
		for (const FProvinceId& Province : OutZones[ZoneIndex].Provinces)
		{
			ProvinceToZone.Add(Province, ZoneIndex);
		}
	}

	for (const FProvinceId& Province : InProvinces)
	{
		if (ProvinceToZone.Contains(Province))
		{
			continue;
		}

		TArray<int32> NeighbourZones;

		// A neighbour outside the loaded scenario has no id to test against ProvinceSet below, so that
		// case is answered from the set EnsureAdjacencyLookup filled instead.
		bool bIsBordering = ProvincesAdjacentToUnknown.Contains(Province);

		if (const TArray<FProvinceId>* Neighbours = ProvinceAdjacencies.Find(Province))
		{
			for (const FProvinceId& Neighbour : *Neighbours)
			{
				if (const int32* ZoneIndex = ProvinceToZone.Find(Neighbour))
				{
					NeighbourZones.AddUnique(*ZoneIndex);
				}

				if (!ProvinceSet.Contains(Neighbour))
				{
					bIsBordering = true;
				}
			}
		}

		if (NeighbourZones.IsEmpty())
		{
			FMapNameZone NewZone;
			NewZone.Provinces.Add(Province);
			if (bIsBordering)
			{
				NewZone.Bordering.Add(Province);
			}

			ProvinceToZone.Add(Province, OutZones.Num());
			OutZones.Add(MoveTemp(NewZone));
			continue;
		}

		// Bridging two zones that were only separate because nothing had joined them yet: the province
		// joins the first and every other zone it touches is folded into that one.
		const int32 PrimaryZone = NeighbourZones[0];
		OutZones[PrimaryZone].Provinces.Add(Province);
		ProvinceToZone.Add(Province, PrimaryZone);

		if (bIsBordering)
		{
			OutZones[PrimaryZone].Bordering.Add(Province);
		}

		for (int32 i = 1; i < NeighbourZones.Num(); ++i)
		{
			const int32 MergeZone = NeighbourZones[i];
			if (!OutZones.IsValidIndex(MergeZone) || MergeZone == PrimaryZone)
			{
				continue;
			}

			for (const FProvinceId& MergedProvince : OutZones[MergeZone].Provinces)
			{
				OutZones[PrimaryZone].Provinces.Add(MergedProvince);
				ProvinceToZone[MergedProvince] = PrimaryZone;
			}

			OutZones[PrimaryZone].Bordering.Append(OutZones[MergeZone].Bordering);

			// Emptied rather than removed: the indices in ProvinceToZone and NeighbourZones would all
			// shift. The empty zones are dropped in one pass at the end.
			OutZones[MergeZone].Provinces.Empty();
			OutZones[MergeZone].Bordering.Empty();
		}
	}

	OutZones.RemoveAll([](const FMapNameZone& InZone) { return InZone.Provinces.IsEmpty(); });
	return true;
}

void ARtsWorldMap::GenerateCountryNames()
{
	for (auto [NationId, WorldMapNationData] : NationNames)
		GenerateCountryName(NationId, WorldMapNationData);
}

void ARtsWorldMap::GenerateCountryName(const FNationId& Id, const FWorldMapNationData& Data)
{
	TArray<FNationId> TraceOverTags = {};
	TraceOverTags.Add(Id);

	if (!true) // Placeholder for when not to render a countries name (for example, too small, or subject)
	{
		return;
	}

	TArray<FProvinceId> Provinces = Data.OwnedProvinces;
	/*
	 * For later
	 *
	 * for (Subject : Data.Subjects) Provinces.Append(Subject.Provinces);
	 */

	TArray<FMapNameZone> Zones = {};
	const bool bSuccess = GenerateMapNameZones(Provinces, Zones);
	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create map name zones"));
		return;
	}

	for (const FMapNameZone& Zone : Zones)
		GenerateCountryNameForZone(Id, Data, Zone, TraceOverTags);
}

void ARtsWorldMap::GenerateCountryNameForZone(const FNationId& Id, const FWorldMapNationData& Data, const FMapNameZone& Zone, const TArray<FNationId>& TraceOverTags)
{
	// Initialize variables
	int32 LargestIndex = 0;
	int32 LargestIndex_0 = 0;
	int32 Num = 0;
	TArray<FVector2D> Points = {};
	FVector2D LargestPoint01 = {0.f, 0.f};
	FVector2D LargestPoint02 = {0.f, 0.f};
	float LargestDistance = 0;
	FVector2D Average = {0.f, 0.f};
	// Find All Locations For Convex Hull & Find Average and Two furthest points
	// A zone with no border at all (every neighbour is the owner's own) still has to be labelled, so
	// it falls back to the whole zone rather than dividing by an empty Points below — that produced a
	// NaN average, and the NaN survived all the way into a texture lookup as an out-of-range index.
	const TArray<FProvinceId>& Outline = Zone.Bordering.IsEmpty() ? Zone.Provinces : Zone.Bordering;
	if (Outline.IsEmpty())
	{
		return;
	}

	for (const FProvinceId& Bordering : Outline)
	{
		FVector2D WorldPosition;
		const bool Found = FindUnitPositonOfProvince(Bordering, WorldPosition);
		if (!Found)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find province's unit position"));
			return;
		}
		Points.Add(WorldPosition);
	}
	for (const FVector2D& Point : Points)
	{
		Average = Point + Average;
		for (const FVector2D& InnerPoint : Points)
		{
			const float Squared = UKismetMathLibrary::Vector_DistanceSquared(UVToLocation(InnerPoint), UVToLocation(Point));
			if (Squared > LargestDistance)
			{
				LargestDistance = Squared;
				LargestPoint01 = InnerPoint;
				LargestPoint02 = Point;
			}
		}
	}
	Average = Average / FVector2D(Points.Num(), Points.Num());
	// Add Name
	{
		ACountryName* Spawned = SpawnMapName();
		if (!Spawned)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not spawn map name"));
			return;
		}
		
		const bool OneSmallerTwo = LargestPoint01.X < LargestPoint02.X;
		const TArray<FVector> PointsArray = {
			UVToLocation(OneSmallerTwo ? LargestPoint02 : LargestPoint01),
			UVToLocation(Average),
			UVToLocation(OneSmallerTwo ? LargestPoint01 : LargestPoint02)
		};
		
		Spawned->Initialize(PointsArray, Data.Name, this, TraceOverTags);
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
