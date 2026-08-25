#include "BuildMenu/BuildMenuWidgetViewModel.h"

#include "Buildings/BuildingCategoryViewModel.h"

void UBuildMenuWidgetViewModel::SetOpen(const bool bInOpen)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsOpen, bInOpen);
}

void UBuildMenuWidgetViewModel::SetCategories(const TArray<UBuildingCategoryViewModel*>& InCategories)
{
	UE_MVVM_SET_PROPERTY_VALUE(Categories, TArray<TObjectPtr<UBuildingCategoryViewModel>>(InCategories));
}

void UBuildMenuWidgetViewModel::SetCategoryCount(const int32 InCategoryCount)
{
	UE_MVVM_SET_PROPERTY_VALUE(CategoryCount, InCategoryCount);
}

void UBuildMenuWidgetViewModel::SetSelectedBuildingName(const FText& InSelectedBuildingName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingName, InSelectedBuildingName);
}

void UBuildMenuWidgetViewModel::SetSelectedBuildingCost(const int32 InSelectedBuildingCost)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingCost, InSelectedBuildingCost);
}
