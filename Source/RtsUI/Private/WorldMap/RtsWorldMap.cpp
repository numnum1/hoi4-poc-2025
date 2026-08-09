#include "WorldMap/RtsWorldMap.h"

#include "BA_DataFunctions.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Provinces.h"
#include "RtsFacadeSubsystem.h"
#include "WorldMap/WorldMapWidget.h"

void ARtsWorldMap::SetWorldMapWidget(UWorldMapWidget* InWidget)
{
	WorldMapWidget = InWidget;
}

void ARtsWorldMap::SelectProvinceAt(const FVector2D InUV)
{
	if (WorldMapWidget)
	{
		WorldMapWidget->HandleProvinceClicked(ResolveProvinceAt(InUV));
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

	UProvinces* Provinces = URtsFacadeSubsystem::Get(this) ? URtsFacadeSubsystem::Get(this)->GetProvincesContainer() : nullptr;
	return Provinces ? Provinces->FindByMapId(ProvinceMapId) : FProvinceId();
}
