#include "Organizations/SelectablePositionViewModel.h"

void USelectablePositionViewModel::SetSource(const int32 InPositionIndex, const FOrganizationId InOrganizationId,
                                             const FText& InDisplayName, const FSlateBrush& InIcon)
{
	PositionIndex = InPositionIndex;
	OrganizationId = InOrganizationId;
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, InDisplayName);
	UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon);
}

void USelectablePositionViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
