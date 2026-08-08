#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecruitCharacterModalWidget.generated.h"

class UPlayerCharacterViewModel;

/**
 * The recruit-character modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Same shape as UJoinOrganizationModalWidget: what this class pushes in
 * (SetPlayerViewModel) is kept only as a Property Path source for one-way Function bindings
 * (RecruitableCharacters, ModalErrorText, CanConfirmModal, ...) and Event bindings for
 * Confirm/Cancel straight to UPlayerCharacterViewModel::ConfirmModal/CloseModal. Which character is
 * picked is set by calling SelectRecruitTarget on the played character view model directly from
 * each row's OnClicked — no logic lives in C++ here.
 */
UCLASS(Abstract)
class RTSUI_API URecruitCharacterModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by the owning view once it has its view model. */
	UFUNCTION(BlueprintCallable, Category="Recruit Character Modal")
	void SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel);

	UFUNCTION(BlueprintPure, Category="Recruit Character Modal")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;
};
