#include "RtsWorldMap.h"

#include "BA_DataFunctions.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsWorldMap, Log, All);

namespace
{
	int32 PackPixelColor(const uint8 InRed, const uint8 InGreen)
	{
		return (static_cast<int32>(InRed) << 8) | static_cast<int32>(InGreen);
	}
}

void ARtsWorldMap::OnConstruction(const FTransform& Transform)
{
	MapDynamicMaterials.Empty();

	Super::OnConstruction(Transform);
}

void ARtsWorldMap::EnsureProvinceLookup()
{
	if (!MapIdToPixelColor.IsEmpty())
	{
		return;
	}

	const UDataTable* ProvinceMapData = GetProvinceMapData();
	if (ProvinceMapData == nullptr)
	{
		if (!bLookupFailureLogged)
		{
			bLookupFailureLogged = true;
			UE_LOG(LogRtsWorldMap, Error,
				TEXT("%s: GetProvinceMapData returned nothing. The map Blueprint has to implement it, "
					 "returning the province DataTable it already holds."), *GetName());
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

		MapIdToPixelColor.Add(MapData->Name, MapData->Color);
		PixelColorToMapId.Add(PackPixelColor(MapData->Color.R, MapData->Color.G), MapData->Name);
	}
}

FProvinceId ARtsWorldMap::ResolveProvinceAt(const FVector2D InUV)
{
	EnsureProvinceLookup();

	UTexture2D* ProvinceIdTexture = GetProvinceIdTexture();
	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (ProvinceIdTexture == nullptr || Provinces == nullptr || PixelColorToMapId.IsEmpty())
	{
		return FProvinceId();
	}

	const int32 Width = ProvinceIdTexture->GetSizeX();
	const int32 Height = ProvinceIdTexture->GetSizeY();
	const int32 X = FMath::Clamp(FMath::FloorToInt(InUV.X * Width), 0, Width - 1);
	const int32 Y = FMath::Clamp(FMath::FloorToInt(InUV.Y * Height), 0, Height - 1);

	FColor PixelColor;
	UBA_DataFunctions::FindColor(ProvinceIdTexture, X, Y, PixelColor);

	const FName* MapId = PixelColorToMapId.Find(PackPixelColor(PixelColor.R, PixelColor.G));
	return MapId ? Provinces->FindByMapId(*MapId) : FProvinceId();
}

void ARtsWorldMap::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	EnsureProvinceLookup();

	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (Provinces == nullptr || MapIdToPixelColor.IsEmpty())
	{
		return;
	}

	TArray<FDataLUT> DataLUT;
	DataLUT.Reserve(InVisuals.Num());

	for (const TPair<FProvinceId, FProvinceVisuals>& Entry : InVisuals)
	{
		const FColorRG* PixelColor = MapIdToPixelColor.Find(Provinces->GetDisplayInfo(Entry.Key).MapId);
		if (PixelColor == nullptr)
		{
			continue;
		}

		FDataLUT Row;
		Row.ID = *PixelColor;
		Row.Primary = Entry.Value.PrimaryColor;
		Row.Secondary = Entry.Value.GetEffectiveSecondaryColor();
		Row.National = Entry.Value.GetEffectiveNationalColor();
		DataLUT.Add(Row);
	}

	if (DataLUT.IsEmpty())
	{
		return;
	}

	// Never clears: the caller sends the provinces it wants changed, not the whole world, and
	// clearing would black out every province missing from this batch.
	WriteToLUTs(DataLUT, /*ClearLUTs=*/false, /*UpdateNational=*/true);
}

ARtsWorldMap* ARtsWorldMap::GetRtsWorldMap(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (World == nullptr)
	{
		return nullptr;
	}

	// Not ABA_Map::GetMap: that caches the first ABA_Map it finds in a static shared across worlds
	// and only casts afterwards, so a stale or unrelated map hands back a null cast here.
	return Cast<ARtsWorldMap>(UGameplayStatics::GetActorOfClass(World, ARtsWorldMap::StaticClass()));
}
