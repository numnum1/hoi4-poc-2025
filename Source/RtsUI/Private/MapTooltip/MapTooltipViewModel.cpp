#include "MapTooltip/MapTooltipViewModel.h"

void UMapTooltipViewModel::SetContent(const FMapTooltipContent& InContent)
{
	UE_MVVM_SET_PROPERTY_VALUE(Headline, InContent.Headline);
	UE_MVVM_SET_PROPERTY_VALUE(Content, InContent.Content);
}
