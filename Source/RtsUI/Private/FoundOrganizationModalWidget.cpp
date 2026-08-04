#include "FoundOrganizationModalWidget.h"

#include "FoundOrganizationModalWidgetViewModel.h"

void UFoundOrganizationModalWidget::SetViewModel(UFoundOrganizationModalWidgetViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void UFoundOrganizationModalWidget::CommitName(const FText& InName)
{
	if (ViewModel)
	{
		ViewModel->SetOrganizationName(InName);
	}
}

void UFoundOrganizationModalWidget::Confirm()
{
	if (ViewModel)
	{
		ViewModel->CallOnConfirmClicked();
	}
}

void UFoundOrganizationModalWidget::Cancel()
{
	if (ViewModel)
	{
		ViewModel->CallOnCancelClicked();
	}
}
