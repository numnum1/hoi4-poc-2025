#include "PlayedCharacterView.h"

#include "FoundOrganizationModalWidget.h"
#include "PlayerCharacterViewModel.h"

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
}
