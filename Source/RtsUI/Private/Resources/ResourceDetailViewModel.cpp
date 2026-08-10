#include "Resources/ResourceDetailViewModel.h"

#include "Resources/RtsResources.h"

void UResourceDetailViewModel::SetSource(const EResourceType InResource, const EResourceDetail InDetail,
                                          const int32 InAmount, const FText& InAmountText)
{
	DetailType = InDetail;
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, URtsResources::GetDetailName(InResource, InDetail));
	UE_MVVM_SET_PROPERTY_VALUE(Icon, URtsResources::GetIcon(InResource));
	UE_MVVM_SET_PROPERTY_VALUE(Amount, InAmount);
	UE_MVVM_SET_PROPERTY_VALUE(AmountText, InAmountText);
}
