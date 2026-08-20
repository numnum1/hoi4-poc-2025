#include "WorldMapWidget.h"

#include "RtsWorldMap.h"

DEFINE_LOG_CATEGORY_STATIC(LogWorldMapWidget, Log, All);

void UWorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResolveWorldMap();
}

void UWorldMapWidget::NativeDestruct()
{
	if (WorldMap)
	{
		WorldMap->OnProvinceClicked.RemoveAll(this);
		WorldMap = nullptr;
	}

	Super::NativeDestruct();
}

ARtsWorldMap* UWorldMapWidget::ResolveWorldMap()
{
	if (WorldMap)
	{
		return WorldMap;
	}

	WorldMap = ARtsWorldMap::GetRtsWorldMap(this);
	if (WorldMap == nullptr)
	{
		// Worth saying out loud: without the map the widget still lays out, it just swallows every
		// color and never reports a click, which looks like broken selection rather than a level
		// missing its map actor.
		if (!bMissingMapLogged)
		{
			bMissingMapLogged = true;
			UE_LOG(LogWorldMapWidget, Error,
				TEXT("%s found no ARtsWorldMap in the level, so the map neither paints nor reports clicks."),
				*GetName());
		}
		return nullptr;
	}

	// Bound where the map is first found rather than in NativeConstruct: the widget can be built
	// before the level's actors are, and a binding attempted only there would then never happen.
	WorldMap->OnProvinceClicked.AddDynamic(this, &UWorldMapWidget::HandleProvinceClicked);
	return WorldMap;
}

void UWorldMapWidget::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	if (ARtsWorldMap* Map = ResolveWorldMap())
	{
		Map->SetProvinceVisuals(InVisuals);
	}
}

void UWorldMapWidget::SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData)
{
	if (ARtsWorldMap* Map = ResolveWorldMap())
	{
		Map->SetNationData(InNationData);
	}
}

void UWorldMapWidget::HandleProvinceClicked(const FProvinceId InProvinceId)
{
	OnProvinceClicked.Broadcast(InProvinceId);
}
