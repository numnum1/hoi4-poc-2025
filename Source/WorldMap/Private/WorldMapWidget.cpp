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

	ApplyOverriddenVisuals();
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
