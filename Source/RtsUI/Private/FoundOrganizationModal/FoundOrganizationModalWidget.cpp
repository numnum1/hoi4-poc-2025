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
	if (PlayerViewModel == nullptr || FormViewModel == nullptr)
	{
		return;
	}

	// Reported as-is, without checking the form's IsValid first: the handler re-runs both gates on
	// exactly these values anyway, and a second gate here would be a copy to keep in sync.
	PlayerViewModel->HandleFoundOrganizationConfirmed(
		FormViewModel->GetOrganizationName(),
		FormViewModel->GetPendingHeadquartersType(),
		FormViewModel->GetSelectedProvinceId());
}

void UFoundOrganizationModalWidget::Cancel()
{
	if (PlayerViewModel == nullptr)
	{
		return;
	}

	PlayerViewModel->CloseModal();
}
