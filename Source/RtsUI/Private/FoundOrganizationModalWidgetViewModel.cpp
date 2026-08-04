#include "FoundOrganizationModalWidgetViewModel.h"

#include "BuildingKindViewModel.h"

void UFoundOrganizationModalWidgetViewModel::SetOrganizationName(const FText& InName)
{
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, InName);
}

void UFoundOrganizationModalWidgetViewModel::SetHeadquartersTypes(const TArray<UBuildingKindViewModel*>& InHeadquartersTypes)
{
	TArray<TObjectPtr<UBuildingKindViewModel>> NewTypes(InHeadquartersTypes);
	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypes, NewTypes);
	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypeCount, NewTypes.Num());
}

void UFoundOrganizationModalWidgetViewModel::SetSelectedHeadquartersType(UBuildingKindViewModel* InSelectedHeadquartersType)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedHeadquartersType, InSelectedHeadquartersType);
}

void UFoundOrganizationModalWidgetViewModel::SetSelectedProvinceName(const FText& InSelectedProvinceName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName, InSelectedProvinceName);
}

void UFoundOrganizationModalWidgetViewModel::SetPendingCost(const int32 InPendingCost)
{
	UE_MVVM_SET_PROPERTY_VALUE(PendingCost, InPendingCost);
}

void UFoundOrganizationModalWidgetViewModel::SetModalErrorText(const FText& InModalErrorText)
{
	UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, InModalErrorText);
}

void UFoundOrganizationModalWidgetViewModel::SetCanConfirm(const bool bInCanConfirm)
{
	UE_MVVM_SET_PROPERTY_VALUE(bCanConfirm, bInCanConfirm);
}

void UFoundOrganizationModalWidgetViewModel::CallOnConfirmClicked()
{
	OnConfirmClicked.Broadcast();
}

void UFoundOrganizationModalWidgetViewModel::CallOnCancelClicked()
{
	OnCancelClicked.Broadcast();
}
