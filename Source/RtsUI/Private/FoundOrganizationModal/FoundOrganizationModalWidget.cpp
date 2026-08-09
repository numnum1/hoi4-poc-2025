#include "FoundOrganizationModal/FoundOrganizationModalWidget.h"

#include "FoundOrganizationModal/FoundOrganizationModalWidgetViewModel.h"
#include "PlayedCharacterView/PlayerCharacterViewModel.h"

void UFoundOrganizationModalWidget::SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel)
{
	PlayerViewModel = InPlayerViewModel;
}

void UFoundOrganizationModalWidget::SetFormViewModel(UFoundOrganizationModalWidgetViewModel* InFormViewModel)
{
	FormViewModel = InFormViewModel;
}

void UFoundOrganizationModalWidget::Confirm()
{
	if (FormViewModel == nullptr)
	{
		return;
	}

	// Reported as-is, without checking the form's IsValid first: whoever handles OnConfirmed
	// re-runs both gates on exactly these values anyway, and a second gate here would be a copy to
	// keep in sync.
	OnConfirmed.Broadcast(
		FormViewModel->GetOrganizationName(),
		FormViewModel->GetPendingHeadquartersType(),
		FormViewModel->GetSelectedProvinceId());
}

void UFoundOrganizationModalWidget::Cancel()
{
	OnCancelled.Broadcast();
}
