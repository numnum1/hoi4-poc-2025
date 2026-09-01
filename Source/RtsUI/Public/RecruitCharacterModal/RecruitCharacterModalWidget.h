#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecruitCharacterModalWidget.generated.h"

class URtsHUDViewModel;

/**
 * The recruit-character modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Same shape as UJoinOrganizationModalWidget: what this class pushes in
 * (SetHUDViewModel) is kept only as a Property Path source for one-way Function bindings
 * (RecruitableCharacters, ModalErrorText, CanConfirmModal, ...) and Event bindings for
 * Confirm/Cancel straight to URtsHUDViewModel::ConfirmModal/CloseModal. Which character is
 * picked is set by calling SelectRecruitTarget on the HUD view model directly from
 * each row's OnClicked — no logic lives in C++ here.
 */
UCLASS(Abstract)
class RTSUI_API URecruitCharacterModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by URtsHUDWidget, which owns the view model. */
	UFUNCTION(BlueprintCallable, Category="Recruit Character Modal")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Recruit Character Modal")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;
};
