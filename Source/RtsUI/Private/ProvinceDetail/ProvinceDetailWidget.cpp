#include "ProvinceDetail/ProvinceDetailWidget.h"

#include "PlayedCharacterView/PlayerCharacterViewModel.h"

void UProvinceDetailWidget::SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel)
{
	PlayerViewModel = InPlayerViewModel;
}

void UProvinceDetailWidget::ViewOrganization(const FOrganizationId InOrganizationId)
{
	if (PlayerViewModel == nullptr || !InOrganizationId.IsValid())
	{
		return;
	}

	PlayerViewModel->ViewOrganization(InOrganizationId);
}
