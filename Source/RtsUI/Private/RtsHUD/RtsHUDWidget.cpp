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

	// Initialize broadcasts on its own, but the map actor may not have resolved its province lookup
	// yet that early — the first tick repaints regardless, and by then it has.
	bProvinceVisualsDirty = true;
}

void URtsHUDWidget::NativeDestruct()
{
	if (HUDViewModel)
	{
		HUDViewModel->RemoveAllFieldValueChangedDelegates(this);
	}

	Super::NativeDestruct();
}

void URtsHUDWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bProvinceVisualsDirty)
	{
		bProvinceVisualsDirty = false;
		PushProvinceVisuals();
	}
}

void URtsHUDWidget::HandleProvinceVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId)
{
	bProvinceVisualsDirty = true;
}

void URtsHUDWidget::PushProvinceVisuals()
{
	// BindWidgetOptional leaves this null without complaint, and a HUD Blueprint whose map widget is
	// named anything else then shows an unpainted map with nothing pointing at the cause.
	if (WorldMapWidget == nullptr)
	{
		if (!bMissingMapWidgetLogged)
		{
			bMissingMapWidgetLogged = true;
			UE_LOG(LogTemp, Error,
				TEXT("%s has no widget named WorldMapWidget, so the map is never handed its colors."),
				*GetName());
		}
		return;
	}

	if (HUDViewModel)
	{
		WorldMapWidget->SetProvinceVisuals(HUDViewModel->GetProvinceVisuals());
	}
}
