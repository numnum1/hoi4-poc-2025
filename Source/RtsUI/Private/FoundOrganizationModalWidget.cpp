#include "FoundOrganizationModalWidget.h"

#include "PlayerCharacterViewModel.h"

void UFoundOrganizationModalWidget::SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel)
{
	PlayerViewModel = InPlayerViewModel;
}

void UFoundOrganizationModalWidget::CommitName(const FText& InName)
{
	if (PlayerViewModel)
	{
		PlayerViewModel->SetPendingOrganizationName(InName);
	}
}

void UFoundOrganizationModalWidget::Confirm()
{
	if (PlayerViewModel)
	{
		PlayerViewModel->ConfirmModal();
	}
}

void UFoundOrganizationModalWidget::Cancel()
{
	if (PlayerViewModel)
	{
		PlayerViewModel->CloseModal();
	}
}
