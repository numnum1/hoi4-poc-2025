#include "RtsHUD/RtsHUDWidget.h"

#include "RtsHUD/RtsHUDViewModel.h"
#include "WorldMapWidget.h"

void URtsHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (HUDViewModel == nullptr)
	{
		HUDViewModel = NewObject<URtsHUDViewModel>(this);
	}

	HUDViewModel->AddFieldValueChangedDelegate(
		URtsHUDViewModel::FFieldNotificationClassDescriptor::GetProvinceVisuals,
		INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &URtsHUDWidget::HandleProvinceVisualsChanged));

	HUDViewModel->Initialize();

	// Initialize's own broadcast happens before the delegate above could matter for the very first
	// paint only if the map is already there; pushing once here covers the ordinary case where the
	// scenario is loaded and nothing will change again on its own.
	PushProvinceVisuals();
}

void URtsHUDWidget::NativeDestruct()
{
	if (HUDViewModel)
	{
		HUDViewModel->RemoveAllFieldValueChangedDelegates(this);
	}

	Super::NativeDestruct();
}

void URtsHUDWidget::HandleProvinceVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId)
{
	PushProvinceVisuals();
}

void URtsHUDWidget::PushProvinceVisuals()
{
	if (WorldMapWidget && HUDViewModel)
	{
		WorldMapWidget->SetProvinceVisuals(HUDViewModel->GetProvinceVisuals());
	}
}
