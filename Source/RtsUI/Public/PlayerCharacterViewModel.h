#pragma once

#include "CoreMinimal.h"
#include "ActionType.h"
#include "CharacterViewModel.h"
#include "PlayerModal.h"
#include "RtsActionContext.h"
#include "RtsActionInput.h"
#include "PlayerCharacterViewModel.generated.h"

class ARtsPlayer;
class UActionViewModel;
class USelectablePositionViewModel;

/**
 * The played character, plus the widget state around it: which of the character's positions is
 * currently selected (or "self"), and the action list that goes with that selection.
 *
 * Everything UCharacterViewModel already exposes (name, portrait, abilities, positions, ...) is
 * inherited unchanged — this class adds only what the simulation has no notion of and the UI has
 * to invent itself. That is the line between RtsViewModels and RtsUI: a value derived from the
 * simulation belongs on the base class in RtsViewModels, a value the UI made up (a selection, a
 * confirmation dialog's open/closed state, ...) belongs here.
 *
 * The picker the player selects with is SelectablePositions — one lean row per position, clicked
 * back up through HandlePositionClicked. Clicking the selected row clears the selection: the player
 * drops back to acting as themselves and no row is selected any more.
 *
 * The action list lives here rather than in a view model of its own precisely because it is a
 * function of that selection: the scope an action is offered for *is* SelectedPositionIndex, so a
 * separate object would only hold a second copy of it to keep in sync by hand. Everything that is
 * not the selection — which actions exist, whether one is allowed, what running it does — is a pure
 * function in URtsActions.
 *
 * Call InitializeForPlayer() once — UPlayedCharacterView does this in NativeOnInitialized, right
 * after constructing the instance — instead of the inherited Initialize(FCharacterId). It finds the
 * local ARtsPlayer, follows whichever character it currently controls, and re-initializes itself
 * automatically when the player switches characters.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UPlayerCharacterViewModel : public UCharacterViewModel
{
	GENERATED_BODY()

public:
	/** Finds the local player and starts following whichever character it controls. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel")
	void InitializeForPlayer();

	/** Act as the character themselves. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel")
	void SelectSelf();

	/** Act as one of the character's positions, indexed as GetPositions() does. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel")
	void SelectPosition(int32 InPositionIndex);

	//~ Modals -------------------------------------------------------------------------------------

	/** Opens a modal, resetting whatever the previous one had collected. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void OpenModal(EPlayerModal InModal);

	/** Closes the open modal without running anything. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void CloseModal();

	/** Controlled input: the modal's text box pushes every edit here, and gets validation back. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void SetPendingOrganizationName(const FText& InName);

	/** Runs the open modal's action after re-checking both gates, then closes it. Does nothing if
	 * either gate refuses — the widget's last pushed bCanConfirmModal is never trusted. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void ConfirmModal();

protected:
	//~ Widget state — invented by the UI, not derived from the simulation ------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 SelectedPositionIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsActingAsSelf, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	bool bIsActingAsSelf = true;

	/** The picker row that is currently selected, or null while acting as self — the same state
	 * SelectedPositionIndex holds, as the object a widget can bind a highlight against. Written in
	 * exactly one place (ApplySelection) so the two can never disagree. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TObjectPtr<USelectablePositionViewModel> SelectedPosition;

	/** PM_None is a real state, so no separate "any modal open" flag exists to fall out of sync. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	EPlayerModal ActiveModal = EPlayerModal::PM_None;

	/** So a widget can drive its visibility without an enum comparison node. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsFoundOrganizationModalOpen, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bIsFoundOrganizationModalOpen = false;

	/** What the player has typed into the found-organization modal so far. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FText PendingOrganizationName;

	/** Empty while the current input is usable. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FText ModalErrorText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=CanConfirmModal, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bCanConfirmModal = false;

	//~ Derived from the selection above -----------------------------------------------------------

	/** One picker row per position the character holds, in the same order as GetPositions(). */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<USelectablePositionViewModel>> SelectablePositions;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 SelectablePositionCount = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UActionViewModel>> Actions;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 ActionCount = 0;

public:
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
	bool IsActingAsSelf() const { return bIsActingAsSelf; }
	USelectablePositionViewModel* GetSelectedPosition() const { return SelectedPosition; }
	const TArray<TObjectPtr<USelectablePositionViewModel>>& GetSelectablePositions() const { return SelectablePositions; }
	int32 GetSelectablePositionCount() const { return SelectablePositionCount; }
	EPlayerModal GetActiveModal() const { return ActiveModal; }
	bool IsFoundOrganizationModalOpen() const { return bIsFoundOrganizationModalOpen; }
	FText GetPendingOrganizationName() const { return PendingOrganizationName; }
	FText GetModalErrorText() const { return ModalErrorText; }
	bool CanConfirmModal() const { return bCanConfirmModal; }
	const TArray<TObjectPtr<UActionViewModel>>& GetActions() const { return Actions; }
	int32 GetActionCount() const { return ActionCount; }

protected:
	virtual void Refresh_Implementation() override;
	virtual void BeginDestroy() override;

private:
	/** The context the action catalogue is queried and gated with, built straight from the current
	 * selection. Never stored, so there is no second copy of the selection to keep in sync. */
	FRtsActionContext MakeContext() const;

	/** Same idea for the modal's collected values — built on demand, never stored twice. */
	FRtsActionInput MakeInput() const;

	/** Which action the open modal is collecting input for. EA_FoundOrganization is meaningless
	 * while ActiveModal is PM_None, so callers check that first. */
	EAction GetModalAction() const;

	/** The one writer of SelectedPositionIndex/bIsActingAsSelf/SelectedPosition. INDEX_NONE means
	 * acting as self. Refreshes the action list, which is a function of exactly this. */
	void ApplySelection(int32 InPositionIndex);

	void RefreshSelectablePositions();

	void RefreshActions();

	/** Re-runs the input gate and pushes the result into ModalErrorText/bCanConfirmModal. */
	void RefreshModalValidation();

	UFUNCTION()
	void HandleActionClicked(UActionViewModel* ClickedAction);

	/** Clicking the row that is already selected clears the selection instead of re-picking it. */
	UFUNCTION()
	void HandlePositionClicked(USelectablePositionViewModel* ClickedPosition);

	UFUNCTION()
	void HandleControlledCharacterChanged();

	UPROPERTY()
	TObjectPtr<ARtsPlayer> Player;

	/** Reused across refreshes so a rebuild that neither adds nor removes an action keeps the same
	 * row objects — the widget's list does not churn and the OnClicked binding survives. */
	UPROPERTY()
	TMap<EAction, TObjectPtr<UActionViewModel>> ActionEntryCache;

	/** Same idea, one row per position index — a refresh that neither adds nor removes a position
	 * keeps the same row objects, so the picker does not churn and the bindings survive. */
	UPROPERTY()
	TArray<TObjectPtr<USelectablePositionViewModel>> SelectablePositionCache;
};
