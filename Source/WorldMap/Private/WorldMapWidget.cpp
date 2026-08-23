#include "WorldMapWidget.h"

#include "RtsWorldMap.h"
#include "RtsWorldMapViewModel.h"
#include "WorldMapSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogWorldMapWidget, Log, All);

void UWorldMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Created here rather than left to a "Create Instance" entry in the Blueprint: the Blueprint
	// binds to this one through GetWorldMapViewModel, and a second instance created by the MVVM
	// panel would sit there uninitialized and never reach the map.
	if (WorldMapViewModel == nullptr)
	{
		WorldMapViewModel = NewObject<URtsWorldMapViewModel>(this);
	}
}

void UWorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResolveWorldMap();
}

void UWorldMapWidget::NativeDestruct()
{
	if (WorldMap)
	{
		WorldMap->OnPixelClicked.RemoveAll(this);
		WorldMap->OnHoveredProvinceIdChanged.RemoveAll(this);
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
	WorldMap->OnPixelClicked.AddDynamic(this, &UWorldMapWidget::HandlePixelClicked);
	WorldMap->OnHoveredProvinceIdChanged.AddDynamic(this, &UWorldMapWidget::HandleHoveredProvinceIdChanged);
	return WorldMap;
}

void UWorldMapWidget::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	ResolveWorldMap();

	if (WorldMapViewModel)
	{
		WorldMapViewModel->SetProvinceVisuals(InVisuals);
	}
}

void UWorldMapWidget::SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData)
{
	ResolveWorldMap();

	if (WorldMapViewModel)
	{
		WorldMapViewModel->SetNationData(InNationData);
	}
}

void UWorldMapWidget::SetHoverHighlightEnabled(const bool bInEnabled)
{
	ResolveWorldMap();

	if (WorldMapViewModel)
	{
		WorldMapViewModel->SetHoverHighlightEnabled(bInEnabled);
	}
}

void UWorldMapWidget::HandlePixelClicked(const FMapPixelId InPixelId)
{
	const UWorldMapSubsystem* Subsystem = UWorldMapSubsystem::Get(this);
	OnProvinceClicked.Broadcast(Subsystem ? Subsystem->FindProvince(InPixelId) : FProvinceId());
}

void UWorldMapWidget::HandleHoveredProvinceIdChanged(const FProvinceId InProvinceId)
{
	OnHoveredProvinceIdChanged.Broadcast(InProvinceId);
}
