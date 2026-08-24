#include "PlayedCharacterView/PlayedCharacterView.h"

#include "FoundOrganizationModal/FoundOrganizationModalWidget.h"
#include "JoinOrganizationModal/JoinOrganizationModalWidget.h"
#include "OrganizationDetail/OrganizationDetailWidget.h"
#include "PlayedCharacterView/PlayerCharacterViewModel.h"
#include "ProvinceDetail/ProvinceDetailWidget.h"
#include "RecruitCharacterModal/RecruitCharacterModalWidget.h"
#include "RtsHUD/RtsHUDViewModel.h"

void UPlayedCharacterView::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PlayerViewModel == nullptr)
	{
		PlayerViewModel = NewObject<UPlayerCharacterViewModel>(this);
		PlayerViewModel->InitializeForPlayer();
	}

	if (JoinOrganizationModal)
	{
		JoinOrganizationModal->SetPlayerViewModel(PlayerViewModel);
	}

	if (ProvinceDetail)
	{
		ProvinceDetail->SetPlayerViewModel(PlayerViewModel);
	}

	if (OrganizationDetail)
	{
		OrganizationDetail->SetPlayerViewModel(PlayerViewModel);
	}

	if (RecruitCharacterModal)
	{
		RecruitCharacterModal->SetPlayerViewModel(PlayerViewModel);
	}
}

void UPlayedCharacterView::SetHUDViewModel(URtsHUDViewModel* InHUDViewModel)
{
	HUDViewModel = InHUDViewModel;

	if (FoundOrganizationModal)
	{
		FoundOrganizationModal->SetHUDViewModel(HUDViewModel);
	}
}
