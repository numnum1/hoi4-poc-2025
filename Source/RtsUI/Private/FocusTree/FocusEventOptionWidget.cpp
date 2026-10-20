#include "FocusTree/FocusEventOptionWidget.h"

#include "Focuses/FocusEventOptionViewModel.h"
#include "RtsHUD/RtsHUDViewModel.h"

void UFocusEventOptionWidget::SetOption(UFocusEventOptionViewModel* InOption, URtsHUDViewModel* InHUDViewModel)
{
	Option = InOption;
	HUDViewModel = InHUDViewModel;
	OnOptionSet(InOption);
}

void UFocusEventOptionWidget::Click()
{
	if (Option && HUDViewModel)
	{
		HUDViewModel->ChooseFocusEventOption(Option->GetOptionIndex());
	}
}
