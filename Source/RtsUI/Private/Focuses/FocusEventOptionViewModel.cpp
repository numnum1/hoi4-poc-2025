#include "Focuses/FocusEventOptionViewModel.h"

#include "EventOption.h"

void UFocusEventOptionViewModel::SetSource(const int32 InOptionIndex, const FEventOption& InOption)
{
	UE_MVVM_SET_PROPERTY_VALUE(OptionIndex, InOptionIndex);
	UE_MVVM_SET_PROPERTY_VALUE(Text, InOption.Text);
	UE_MVVM_SET_PROPERTY_VALUE(Tooltip, InOption.Tooltip);
}
