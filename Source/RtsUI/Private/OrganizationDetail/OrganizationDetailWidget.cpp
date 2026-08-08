#include "OrganizationDetail/OrganizationDetailWidget.h"

#include "PlayedCharacterView/PlayerCharacterViewModel.h"

void UOrganizationDetailWidget::SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel)
{
	PlayerViewModel = InPlayerViewModel;
}
