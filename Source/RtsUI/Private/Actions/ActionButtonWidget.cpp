#include "Actions/ActionButtonWidget.h"

void UActionButtonWidget::CallOnClicked()
{
	OnClicked.Broadcast();
}
