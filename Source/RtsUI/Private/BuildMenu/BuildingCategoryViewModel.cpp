#include "BuildMenu/BuildingCategoryViewModel.h"

void UBuildingCategoryViewModel::SetDisplay(const FText& InDisplayName, const FText& InDescription, const FSlateBrush& InIcon, const TArray<UBuildMenuEntry*> InEntries)
{
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, InDisplayName);
	UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription);
	UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon);
	UE_MVVM_SET_PROPERTY_VALUE(Entries, InEntries);
}
