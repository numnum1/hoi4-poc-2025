#include "MapTooltip/MapTooltipViewModel.h"

void UMapTooltipViewModel::SetContent(const FMapTooltipContent& InContent)
{
	const bool bHeadlineChanged = UE_MVVM_SET_PROPERTY_VALUE(Headline, InContent.Headline);
	const bool bContentChanged = UE_MVVM_SET_PROPERTY_VALUE(Content, InContent.Content);

	if (bHeadlineChanged || bContentChanged)
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeShow);
	}
}

bool UMapTooltipViewModel::ComputeShow() const
{
	return !Headline.IsEmpty() || !Content.IsEmpty();
}
