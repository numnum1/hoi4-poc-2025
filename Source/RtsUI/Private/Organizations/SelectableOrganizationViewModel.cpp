#include "Organizations/SelectableOrganizationViewModel.h"

void USelectableOrganizationViewModel::SetSource(const FOrganizationId InOrganizationId, const FText& InDisplayName,
                                                  const FSlateBrush& InIcon)
{
	OrganizationId = InOrganizationId;
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, InDisplayName);
	UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon);
}

void USelectableOrganizationViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
