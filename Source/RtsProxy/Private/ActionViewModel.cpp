#include "ActionViewModel.h"

#include "RtsActions.h"

void UActionViewModel::SetAction(const EAction InActionType)
{
	ActionType = InActionType;
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, URtsActions::GetDisplayName(InActionType));
	UE_MVVM_SET_PROPERTY_VALUE(Icon, URtsActions::GetIcon(InActionType));
}

void UActionViewModel::SetEnabled(const bool bInEnabled, const FText& InTooltip)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsEnabled, bInEnabled);
	UE_MVVM_SET_PROPERTY_VALUE(Tooltip, InTooltip);
}

void UActionViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
