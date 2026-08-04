#include "BuildingKindViewModel.h"

#include "BuildingType.h"

void UBuildingKindViewModel::SetSource(UBuildingType* InBuildingType)
{
	BuildingType = InBuildingType;

	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, InBuildingType ? InBuildingType->GetBuildingName() : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(Icon, InBuildingType ? InBuildingType->GetIcon() : FSlateNoResource());

	const int32 NewCost = InBuildingType ? InBuildingType->GetBuildCost() : 0;
	UE_MVVM_SET_PROPERTY_VALUE(Cost, NewCost);
	UE_MVVM_SET_PROPERTY_VALUE(CostText, FText::AsNumber(NewCost));
}

void UBuildingKindViewModel::SetAffordable(const bool bInAffordable)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsAffordable, bInAffordable);
}

void UBuildingKindViewModel::SetSelected(const bool bInSelected)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsSelected, bInSelected);
}

void UBuildingKindViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
