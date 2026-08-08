#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinOrganizationModalWidget.generated.h"

class UPlayerCharacterViewModel;

/**
 * The join-organization modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Lives inside the played character's view for the same reason
 * UFoundOrganizationModalWidget does — it is that character who joins.
 *
 * Same shape as UFoundOrganizationModalWidget: what this class pushes in (SetPlayerViewModel) is
 * kept only as a Property Path source. The Blueprint wires one-way Function bindings from
 * UPlayerCharacterViewModel's getters (JoinableOrganizations, ModalErrorText, CanConfirmModal, ...)
 * onto the modal's own display, and Event bindings for Confirm/Cancel straight to
 * UPlayerCharacterViewModel::ConfirmModal/CloseModal. Which organization is picked is set by
 * calling SelectJoinTarget on the played character view model directly from each row's OnClicked
 * — no logic lives in C++ here, same reason as the found-organization modal.
 */
UCLASS(Abstract)
class RTSUI_API UJoinOrganizationModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by the owning view once it has its view model. */
	UFUNCTION(BlueprintCallable, Category="Join Organization Modal")
	void SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel);

	UFUNCTION(BlueprintPure, Category="Join Organization Modal")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;
};
