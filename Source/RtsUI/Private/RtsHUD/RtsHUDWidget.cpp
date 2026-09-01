#include "RtsHUD/RtsHUDWidget.h"

#include "FoundOrganizationModal/FoundOrganizationModalWidget.h"
#include "JoinOrganizationModal/JoinOrganizationModalWidget.h"
#include "OrganizationDetail/OrganizationDetailWidget.h"
#include "ProvinceDetail/ProvinceDetailWidget.h"
#include "RecruitCharacterModal/RecruitCharacterModalWidget.h"
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

	HUDViewModel->AddFieldValueChangedDelegate(
		URtsHUDViewModel::FFieldNotificationClassDescriptor::GetNationData,
		INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &URtsHUDWidget::HandleNationDataChanged));

	HUDViewModel->Initialize();

	// A child widget is fully initialized before its parent, so every one of these already exists.
	if (FoundOrganizationModal)
	{
		FoundOrganizationModal->SetHUDViewModel(HUDViewModel);
	}

	if (JoinOrganizationModal)
	{
		JoinOrganizationModal->SetHUDViewModel(HUDViewModel);
	}

	if (ProvinceDetail)
	{
		ProvinceDetail->SetHUDViewModel(HUDViewModel);
	}

	if (OrganizationDetail)
	{
		OrganizationDetail->SetHUDViewModel(HUDViewModel);
	}

	if (RecruitCharacterModal)
	{
		RecruitCharacterModal->SetHUDViewModel(HUDViewModel);
	}

	// Bound here rather than in the HUD Blueprint's graph: this class holds both the map and the
	// view model, and what a province click means is a decision, not layout.
	URtsHUDViewModel* ViewModel = HUDViewModel;
	if (UWorldMapWidget* MapWidget = ResolveMapWidget())
	{
		MapWidget->OnProvinceClicked.AddDynamic(ViewModel, &URtsHUDViewModel::SetSelectedProvinceId);
		MapWidget->OnHoveredProvinceIdChanged.AddDynamic(ViewModel, &URtsHUDViewModel::SetHoveredProvinceId);
	}

	// Initialize broadcasts on its own, but the map actor may not have resolved its province lookup
	// yet that early — the first tick repaints regardless, and by then it has.
	bProvinceVisualsDirty = true;
	bNationDataDirty = true;
}

void URtsHUDWidget::NativeDestruct()
{
	if (HUDViewModel)
	{
		HUDViewModel->RemoveAllFieldValueChangedDelegates(this);

		if (WorldMapWidget)
		{
			WorldMapWidget->OnProvinceClicked.RemoveAll(HUDViewModel.Get());
			WorldMapWidget->OnHoveredProvinceIdChanged.RemoveAll(HUDViewModel.Get());
		}
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

	if (bNationDataDirty)
	{
		bNationDataDirty = false;
		PushNationData();
	}
}

void URtsHUDWidget::HandleProvinceVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId)
{
	bProvinceVisualsDirty = true;
}

void URtsHUDWidget::HandleNationDataChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId)
{
	bNationDataDirty = true;
}

UWorldMapWidget* URtsHUDWidget::ResolveMapWidget()
{
	// BindWidgetOptional leaves this null without complaint, and a HUD Blueprint whose map widget is
	// named anything else then shows an unpainted map with nothing pointing at the cause.
	if (WorldMapWidget == nullptr && !bMissingMapWidgetLogged)
	{
		bMissingMapWidgetLogged = true;
		UE_LOG(LogTemp, Error,
			TEXT("%s has no widget named WorldMapWidget, so the map is never handed its colors."),
			*GetName());
	}

	return WorldMapWidget;
}

void URtsHUDWidget::PushProvinceVisuals()
{
	UWorldMapWidget* MapWidget = ResolveMapWidget();
	if (MapWidget && HUDViewModel)
	{
		MapWidget->SetProvinceVisuals(HUDViewModel->GetProvinceVisuals());
	}
}

void URtsHUDWidget::PushNationData()
{
	UWorldMapWidget* MapWidget = ResolveMapWidget();
	if (MapWidget && HUDViewModel)
	{
		MapWidget->SetNationData(HUDViewModel->GetNationData());
	}
}
