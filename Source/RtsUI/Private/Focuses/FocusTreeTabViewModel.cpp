#include "Focuses/FocusTreeTabViewModel.h"

#include "OrganizationFocusTreeAsset.h"

void UFocusTreeTabViewModel::SetSource(const int32 InTreeIndex, const UOrganizationFocusTreeAsset* InTree, const bool bInIsActive)
{
	UE_MVVM_SET_PROPERTY_VALUE(TreeIndex, InTreeIndex);
	UE_MVVM_SET_PROPERTY_VALUE(TreeName, InTree ? InTree->GetTreeName() : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(Icon, InTree ? InTree->GetIcon() : FSlateBrush());
	UE_MVVM_SET_PROPERTY_VALUE(bIsActive, bInIsActive);
}
