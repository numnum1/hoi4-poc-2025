#include "BuildMenu/BuildMenuEntry.h"

#include "BuildingType.h"

void UBuildMenuEntry::Initialize(UBuildingType* InBuildingType)
{
	BuildingType = InBuildingType;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeDisplayName);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeIcon);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeCost);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeCostText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeIsHeadquarters);
}

void UBuildMenuEntry::SetAffordable(const bool bInAffordable)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsAffordable, bInAffordable);
}

void UBuildMenuEntry::SetSelected(const bool bInSelected)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsSelected, bInSelected);
}

FText UBuildMenuEntry::ComputeDisplayName() const
{
	return BuildingType ? BuildingType->GetBuildingName() : FText::GetEmpty();
}

FSlateBrush UBuildMenuEntry::ComputeIcon() const
{
	return BuildingType ? BuildingType->GetIcon() : FSlateBrush();
}

int32 UBuildMenuEntry::ComputeCost() const
{
	return BuildingType ? BuildingType->GetBuildCost() : 0;
}

FText UBuildMenuEntry::ComputeCostText() const
{
	return FText::AsNumber(ComputeCost());
}

bool UBuildMenuEntry::ComputeIsHeadquarters() const
{
	return BuildingType && BuildingType->IsHeadquarters();
}

void UBuildMenuEntry::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
