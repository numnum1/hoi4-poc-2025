#include "FoundOrganizationModal/FoundOrganizationModalWidget.h"

#include "FoundOrganizationModal/FoundOrganizationModalWidgetViewModel.h"
#include "RtsHUD/RtsHUDViewModel.h"

void UFoundOrganizationModalWidget::SetHUDViewModel(URtsHUDViewModel* InHUDViewModel)
{
	HUDViewModel = InHUDViewModel;
}

void UFoundOrganizationModalWidget::SetFormViewModel(UFoundOrganizationModalWidgetViewModel* InFormViewModel)
{
	FormViewModel = InFormViewModel;
}

void UFoundOrganizationModalWidget::Confirm()
{
	if (FormViewModel == nullptr || HUDViewModel == nullptr)
	{
		return;
	}

	// Reported as-is, without checking the verdict first: whoever handles OnConfirmed re-runs both
	// gates on exactly these values anyway, and a second gate here would be a copy to keep in sync.
	OnConfirmed.Broadcast(
		FormViewModel->GetOrganizationName(),
		HUDViewModel->GetSelectedBuildingType(),
		HUDViewModel->GetSelectedProvinceId());
}

void UFoundOrganizationModalWidget::Cancel()
{
	OnCancelled.Broadcast();
}
