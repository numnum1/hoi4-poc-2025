#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinOrganizationModalWidget.generated.h"

class URtsHUDViewModel;

/**
 * The join-organization modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Lives under the HUD for the same reason
 * UFoundOrganizationModalWidget does — it is that character who joins.
 *
 * Same shape as UFoundOrganizationModalWidget: what this class pushes in (SetHUDViewModel) is
 * kept only as a Property Path source. The Blueprint wires one-way Function bindings from
 * URtsHUDViewModel's getters (JoinableOrganizations, ModalErrorText, CanConfirmModal, ...)
 * onto the modal's own display, and Event bindings for Confirm/Cancel straight to
 * URtsHUDViewModel::ConfirmModal/CloseModal. Which organization is picked is set by
 * calling SelectJoinTarget on the HUD view model directly from each row's OnClicked
 * — no logic lives in C++ here, same reason as the found-organization modal.
 */
UCLASS(Abstract)
class RTSUI_API UJoinOrganizationModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by URtsHUDWidget, which owns the view model. */
	UFUNCTION(BlueprintCallable, Category="Join Organization Modal")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Join Organization Modal")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;
};
