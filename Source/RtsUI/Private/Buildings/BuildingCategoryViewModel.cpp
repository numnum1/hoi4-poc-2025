#include "Buildings/BuildingCategoryViewModel.h"

#include "Buildings/RtsBuildingCategories.h"

void UBuildingCategoryViewModel::SetCategory(const EBuildingCategory InCategory)
{
	Category = InCategory;
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, URtsBuildingCategories::GetDisplayName(InCategory));
}

void UBuildingCategoryViewModel::SetKinds(const TArray<TObjectPtr<UBuildingKindViewModel>>& InKinds)
{
	UE_MVVM_SET_PROPERTY_VALUE(Kinds, InKinds);
	UE_MVVM_SET_PROPERTY_VALUE(KindCount, InKinds.Num());
}
