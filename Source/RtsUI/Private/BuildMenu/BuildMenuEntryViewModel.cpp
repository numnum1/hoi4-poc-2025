#include "BuildMenu/BuildMenuEntryViewModel.h"

#include "BuildingType.h"

void UBuildMenuEntryViewModel::Initialize(UBuildingType* InBuildingType)
{
	BuildingType = InBuildingType;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeDisplayName);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeIcon);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeCost);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeCostText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeIsHeadquarters);
}

void UBuildMenuEntryViewModel::SetAffordable(const bool bInAffordable)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsAffordable, bInAffordable);
}

void UBuildMenuEntryViewModel::SetSelected(const bool bInSelected)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsSelected, bInSelected);
}

FText UBuildMenuEntryViewModel::ComputeDisplayName() const
{
	return BuildingType ? BuildingType->GetBuildingName() : FText::GetEmpty();
}

FSlateBrush UBuildMenuEntryViewModel::ComputeIcon() const
{
	return BuildingType ? BuildingType->GetIcon() : FSlateBrush();
}

int32 UBuildMenuEntryViewModel::ComputeCost() const
{
	return BuildingType ? BuildingType->GetBuildCost() : 0;
}

FText UBuildMenuEntryViewModel::ComputeCostText() const
{
	return FText::AsNumber(ComputeCost());
}

bool UBuildMenuEntryViewModel::ComputeIsHeadquarters() const
{
	return BuildingType && BuildingType->IsHeadquarters();
}

void UBuildMenuEntryViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
