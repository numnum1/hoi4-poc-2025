#include "PlayedCharacterView/PlayedCharacterView.h"

#include "FoundOrganizationModal/FoundOrganizationModalWidget.h"
#include "JoinOrganizationModal/JoinOrganizationModalWidget.h"
#include "OrganizationDetail/OrganizationDetailWidget.h"
#include "PlayedCharacterView/PlayerCharacterViewModel.h"
#include "ProvinceDetail/ProvinceDetailWidget.h"
#include "RecruitCharacterModal/RecruitCharacterModalWidget.h"

void UPlayedCharacterView::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PlayerViewModel == nullptr)
	{
		PlayerViewModel = NewObject<UPlayerCharacterViewModel>(this);
		PlayerViewModel->InitializeForPlayer();
	}

	if (FoundOrganizationModal)
	{
		FoundOrganizationModal->SetPlayerViewModel(PlayerViewModel);
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
