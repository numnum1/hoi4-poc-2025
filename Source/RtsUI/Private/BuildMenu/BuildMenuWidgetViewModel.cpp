#include "BuildMenu/BuildMenuWidgetViewModel.h"

#include "BuildMenu/BuildMenuEntryViewModel.h"
#include "BuildMenu/BuildingCategoryViewModel.h"
#include "BuildMenu/RtsBuildingCategories.h"
#include "BuildingType.h"

void UBuildMenuWidgetViewModel::Fill(const TArray<UBuildingType*>& InBuildings)
{
	Buildings = InBuildings;
	RebuildCategories();

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Buildings);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeCategories);
}

void UBuildMenuWidgetViewModel::RebuildCategories()
{
	Categories.Reset();

	for (const EBuildingCategory Category : URtsBuildingCategories::GetCategories())
	{
		TArray<UBuildMenuEntryViewModel*> Entries;

		for (UBuildingType* BuildingType : Buildings)
		{
			if (BuildingType == nullptr || BuildingType->GetCategory() != Category)
			{
				continue;
			}

			UBuildMenuEntryViewModel* Entry = NewObject<UBuildMenuEntryViewModel>(this);
			Entry->Initialize(BuildingType);
			Entries.Add(Entry);
		}

		if (Entries.IsEmpty())
		{
			continue;
		}

		UBuildingCategoryViewModel* Row = NewObject<UBuildingCategoryViewModel>(this);
		Row->SetDisplay(
			URtsBuildingCategories::GetDisplayName(Category),
			URtsBuildingCategories::GetDescription(Category),
			URtsBuildingCategories::GetIcon(Category),
			Entries);

		Categories.Add(Row);
	}
}

TArray<UBuildingCategoryViewModel*> UBuildMenuWidgetViewModel::ComputeCategories() const
{
	TArray<UBuildingCategoryViewModel*> Result;
	Result.Reserve(Categories.Num());

	for (const TObjectPtr<UBuildingCategoryViewModel>& Category : Categories)
	{
		Result.Add(Category);
	}

	return Result;
}
