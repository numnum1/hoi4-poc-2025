#include "WorldMapWidget.h"

#include "RtsWorldMap.h"
#include "WorldMapViewModel.h"

void UWorldMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (WorldMapViewModel == nullptr)
	{
		WorldMapViewModel = NewObject<UWorldMapViewModel>(this);
	}

	WorldMapViewModel->AddFieldValueChangedDelegate(
		UWorldMapViewModel::FFieldNotificationClassDescriptor::OverriddenVisuals,
		INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UWorldMapWidget::HandleOverriddenVisualsChanged));

	// Registering both directions here rather than waiting for the HUD to push us in: the map is a
	// level actor this widget can find on its own, and without the back-channel it drops every click.
	// SetWorldMapWidget repaints, so no separate ApplyOverriddenVisuals call.
	if (ARtsWorldMap* WorldMap = ARtsWorldMap::GetRtsWorldMap(this))
	{
		WorldMap->SetWorldMapWidget(this);
	}
}

void UWorldMapWidget::HandleProvinceClicked(const FProvinceId InProvinceId)
{
	OnProvinceClicked.Broadcast(InProvinceId);
}

void UWorldMapWidget::ApplyOverriddenVisuals()
{
	ARtsWorldMap* WorldMap = ARtsWorldMap::GetRtsWorldMap(this);
	if (WorldMap == nullptr || WorldMapViewModel == nullptr)
	{
		return;
	}

	WorldMap->SetProvinceVisuals(WorldMapViewModel->GetOverriddenVisuals());
}

void UWorldMapWidget::HandleOverriddenVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId)
{
	ApplyOverriddenVisuals();
}
