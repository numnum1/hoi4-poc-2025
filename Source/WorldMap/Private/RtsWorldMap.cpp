#include "RtsWorldMap.h"

#include "BA_DataFunctions.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"
#include "WorldMapWidget.h"

void ARtsWorldMap::SetWorldMapWidget(UWorldMapWidget* InWidget)
{
	WorldMapWidget = InWidget;

	// The widget's first ApplyOverriddenVisuals runs before this actor is reachable when the HUD
	// builds its widgets first, so repaint once the connection exists.
	if (WorldMapWidget)
	{
		WorldMapWidget->ApplyOverriddenVisuals();
	}
}

void ARtsWorldMap::SelectProvinceAt(const FVector2D InUV)
{
	if (WorldMapWidget)
	{
		WorldMapWidget->HandleProvinceClicked(ResolveProvinceAt(InUV));
	}
}

void ARtsWorldMap::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	EnsureProvinceColorLookup();

	TArray<FDataLUT> DataLUT;
	DataLUT.Reserve(InVisuals.Num());

	for (const TPair<FProvinceId, FProvinceVisuals>& Entry : InVisuals)
	{
		const FColorRG* LookupId = ProvinceIdToColorRG.Find(Entry.Key);
		if (LookupId == nullptr)
		{
			continue;
		}

		FDataLUT Row;
		Row.ID = *LookupId;
		Row.Primary = Entry.Value.PrimaryColor;
		Row.Secondary = Entry.Value.SecondaryColor;
		DataLUT.Add(Row);
	}

	WriteToLUTs(DataLUT, /*ClearLUTs=*/true, /*UpdateNational=*/false);
}

void ARtsWorldMap::SetProvinceOverlayColors(const TMap<FProvinceId, FColor>& InColors)
{
	TMap<FProvinceId, FProvinceVisuals> Visuals;
	Visuals.Reserve(InColors.Num());

	for (const TPair<FProvinceId, FColor>& Entry : InColors)
	{
		FProvinceVisuals& Visual = Visuals.Add(Entry.Key);
		Visual.PrimaryColor = Entry.Value;
	}

	SetProvinceVisuals(Visuals);
}

void ARtsWorldMap::ClearProvinceOverlayColors()
{
	WriteToLUTs(TArray<FDataLUT>(), /*ClearLUTs=*/true, /*UpdateNational=*/false);
}

ARtsWorldMap* ARtsWorldMap::GetRtsWorldMap(const UObject* WorldContextObject)
{
	return Cast<ARtsWorldMap>(GetMap(WorldContextObject));
}

void ARtsWorldMap::EnsureProvinceColorLookup()
{
	if (ProvinceIdToColorRG.Num() > 0 || ProvinceMapData == nullptr)
	{
		return;
	}

	UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
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

		const FProvinceId Id = Provinces->FindByMapId(MapData->Name);
		if (Id.IsValid())
		{
			ProvinceIdToColorRG.Add(Id, MapData->Color);
		}
	}
}

FProvinceId ARtsWorldMap::ResolveProvinceAt(const FVector2D InUV) const
{
	if (ProvinceIdTexture == nullptr || ProvinceMapData == nullptr)
	{
		return FProvinceId();
	}

	const int32 Width = ProvinceIdTexture->GetSizeX();
	const int32 Height = ProvinceIdTexture->GetSizeY();
	const int32 X = FMath::Clamp(FMath::FloorToInt(InUV.X * Width), 0, Width - 1);
	const int32 Y = FMath::Clamp(FMath::FloorToInt(InUV.Y * Height), 0, Height - 1);

	FColor UnusedRGBA;
	const FColorRG PixelColor = UBA_DataFunctions::FindColor(ProvinceIdTexture, X, Y, UnusedRGBA);

	FName ProvinceMapId = NAME_None;
	for (const TPair<FName, uint8*>& Row : ProvinceMapData->GetRowMap())
	{
		const FProvinceMapDataGame* MapData = reinterpret_cast<FProvinceMapDataGame*>(Row.Value);
		if (MapData && MapData->Color == PixelColor)
		{
			ProvinceMapId = MapData->Name;
			break;
		}
	}

	if (ProvinceMapId.IsNone())
	{
		return FProvinceId();
	}

	UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	return Provinces ? Provinces->FindByMapId(ProvinceMapId) : FProvinceId();
}
