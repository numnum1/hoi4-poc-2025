#include "Buildings/RtsBuildingCategories.h"

#define LOCTEXT_NAMESPACE "RtsBuildingCategories"

TArray<EBuildingCategory> URtsBuildingCategories::GetCategories()
{
	return
	{
		EBuildingCategory::BC_Administration,
		EBuildingCategory::BC_Religion,
		EBuildingCategory::BC_Industry,
		EBuildingCategory::BC_Commerce
	};
}

FText URtsBuildingCategories::GetDisplayName(const EBuildingCategory InCategory)
{
	switch (InCategory)
	{
	case EBuildingCategory::BC_Administration:
		return LOCTEXT("Administration", "Administration");
	case EBuildingCategory::BC_Religion:
		return LOCTEXT("Religion", "Religion");
	case EBuildingCategory::BC_Industry:
		return LOCTEXT("Industry", "Industry");
	case EBuildingCategory::BC_Commerce:
		return LOCTEXT("Commerce", "Commerce");
	}
	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
