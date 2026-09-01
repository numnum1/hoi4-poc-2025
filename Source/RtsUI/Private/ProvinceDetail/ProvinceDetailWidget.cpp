#include "ProvinceDetail/ProvinceDetailWidget.h"

#include "RtsHUD/RtsHUDViewModel.h"

void UProvinceDetailWidget::SetHUDViewModel(URtsHUDViewModel* InHUDViewModel)
{
	HUDViewModel = InHUDViewModel;
}

void UProvinceDetailWidget::ViewOrganization(const FOrganizationId InOrganizationId)
{
	if (HUDViewModel == nullptr || !InOrganizationId.IsValid())
	{
		return;
	}

	HUDViewModel->ViewOrganization(InOrganizationId);
}
