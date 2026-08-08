#include "PlayedCharacterView.h"

#include "FoundOrganizationModalWidget.h"
#include "JoinOrganizationModalWidget.h"
#include "OrganizationDetailWidget.h"
#include "PlayerCharacterViewModel.h"
#include "ProvinceDetailWidget.h"
#include "RecruitCharacterModalWidget.h"

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
