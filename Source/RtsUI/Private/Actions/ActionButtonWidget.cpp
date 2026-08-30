#include "Actions/ActionButtonWidget.h"

void UActionButtonWidget::SetSource(const FText& InDisplayName, const FSlateBrush& InIcon)
{
	DisplayName = InDisplayName;
	Icon = InIcon;
}

void UActionButtonWidget::CallOnClicked()
{
	OnClicked.Broadcast();
}
