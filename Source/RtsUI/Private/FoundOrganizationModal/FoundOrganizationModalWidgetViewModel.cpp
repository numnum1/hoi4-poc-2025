#include "FoundOrganizationModal/FoundOrganizationModalWidgetViewModel.h"

void UFoundOrganizationModalWidgetViewModel::SetOrganizationName(const FText& InOrganizationName)
{
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, InOrganizationName);
}

void UFoundOrganizationModalWidgetViewModel::SetSelectedBuildingName(const FText& InSelectedBuildingName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingName, InSelectedBuildingName);
}

void UFoundOrganizationModalWidgetViewModel::SetSelectedBuildingCost(const int32 InSelectedBuildingCost)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingCost, InSelectedBuildingCost);
}

void UFoundOrganizationModalWidgetViewModel::SetSelectedProvinceName(const FText& InSelectedProvinceName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName, InSelectedProvinceName);
}

void UFoundOrganizationModalWidgetViewModel::SetErrorText(const FText& InErrorText)
{
	UE_MVVM_SET_PROPERTY_VALUE(ErrorText, InErrorText);
}

void UFoundOrganizationModalWidgetViewModel::SetIsValid(const bool bInIsValid)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, bInIsValid);
}

void UFoundOrganizationModalWidgetViewModel::ResetForm()
{
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, FText::GetEmpty());
}
