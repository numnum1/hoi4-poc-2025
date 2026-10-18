#include "Focuses/FocusNodeViewModel.h"

#include "OrganizationFocusAsset.h"

void UFocusNodeViewModel::SetSource(UOrganizationFocusAsset* InFocus, const FFocusTreeLayoutCell& InCell)
{
	UE_MVVM_SET_PROPERTY_VALUE(Focus, InFocus);
	UE_MVVM_SET_PROPERTY_VALUE(FocusName, InFocus ? InFocus->GetFocusName() : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(Tooltip, InFocus ? InFocus->GetTooltip() : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(Icon, InFocus ? InFocus->GetIcon() : FSlateBrush());
	UE_MVVM_SET_PROPERTY_VALUE(DurationDays, InFocus ? InFocus->GetDurationDays() : 0);
	UE_MVVM_SET_PROPERTY_VALUE(GridRow, InCell.Row);
	UE_MVVM_SET_PROPERTY_VALUE(GridColumn, InCell.Column);
}

void UFocusNodeViewModel::SetStatus(const EOrganizationFocusState InState, const FText& InStateReason, const float InProgress)
{
	UE_MVVM_SET_PROPERTY_VALUE(State, InState);
	UE_MVVM_SET_PROPERTY_VALUE(StateReason, InStateReason);
	UE_MVVM_SET_PROPERTY_VALUE(Progress, InProgress);
}
