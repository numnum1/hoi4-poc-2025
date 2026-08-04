#include "BuildingKindViewModel.h"

#include "BuildingKinds.h"

void UBuildingKindViewModel::SetSource(const EBuildingKind InKind)
{
	BuildingKind = InKind;

	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, UBuildingKinds::GetDisplayName(InKind));
	UE_MVVM_SET_PROPERTY_VALUE(Icon, UBuildingKinds::GetIcon(InKind));

	const int32 NewCost = UBuildingKinds::GetBuildCost(InKind);
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
