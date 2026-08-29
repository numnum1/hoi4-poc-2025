#include "BuildMenu/RtsBuildingCategories.h"

#include "Brushes/SlateNoResource.h"

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

FText URtsBuildingCategories::GetDescription(const EBuildingCategory InCategory)
{
	switch (InCategory)
	{
	case EBuildingCategory::BC_Administration:
		return LOCTEXT("AdministrationDescription", "Seats of bureaucracy and rule.");
	case EBuildingCategory::BC_Religion:
		return LOCTEXT("ReligionDescription", "Temples, shrines and the offices of faith.");
	case EBuildingCategory::BC_Industry:
		return LOCTEXT("IndustryDescription", "Workshops and works that turn raw goods into wares.");
	case EBuildingCategory::BC_Commerce:
		return LOCTEXT("CommerceDescription", "Markets, warehouses and counting houses.");
	}
	return FText::GetEmpty();
}

FSlateBrush URtsBuildingCategories::GetIcon(const EBuildingCategory InCategory)
{
	switch (InCategory)
	{
	case EBuildingCategory::BC_Administration:
	case EBuildingCategory::BC_Religion:
	case EBuildingCategory::BC_Industry:
	case EBuildingCategory::BC_Commerce:
		return FSlateNoResource();
	}
	return FSlateNoResource();
}

#undef LOCTEXT_NAMESPACE
