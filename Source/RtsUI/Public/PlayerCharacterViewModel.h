#pragma once

#include "CoreMinimal.h"
#include "ActionType.h"
#include "CharacterViewModel.h"
#include "RtsActionContext.h"
#include "PlayerCharacterViewModel.generated.h"

class ARtsPlayer;
class UActionViewModel;

/**
 * The played character, plus the widget state around it: which of the character's positions is
 * currently selected (or "self"), and the action list that goes with that selection.
 *
 * Everything UCharacterViewModel already exposes (name, portrait, abilities, positions, ...) is
 * inherited unchanged — this class adds only what the simulation has no notion of and the UI has
 * to invent itself. That is the line between RtsProxy and RtsUI: a value derived from the
 * simulation belongs on the base class in RtsProxy, a value the UI made up (a selection, a
 * confirmation dialog's open/closed state, ...) belongs here.
 *
 * The action list lives here rather than in a view model of its own precisely because it is a
 * function of that selection: the scope an action is offered for *is* SelectedPositionIndex, so a
 * separate object would only hold a second copy of it to keep in sync by hand. Everything that is
 * not the selection — which actions exist, whether one is allowed, what running it does — is a pure
 * function in URtsActions.
 *
 * Call InitializeForPlayer() once (e.g. from a widget's construct event) instead of the inherited
 * Initialize(FCharacterId) — it finds the local ARtsPlayer, follows whichever character it
 * currently controls, and re-initializes itself automatically when the player switches characters.
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

protected:
	//~ Widget state — invented by the UI, not derived from the simulation ------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 SelectedPositionIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsActingAsSelf, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	bool bIsActingAsSelf = true;

	//~ Derived from the selection above -----------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UActionViewModel>> Actions;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 ActionCount = 0;

public:
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
	bool IsActingAsSelf() const { return bIsActingAsSelf; }
	const TArray<TObjectPtr<UActionViewModel>>& GetActions() const { return Actions; }
	int32 GetActionCount() const { return ActionCount; }

protected:
	virtual void Refresh_Implementation() override;
	virtual void BeginDestroy() override;

private:
	/** The context the action catalogue is queried and gated with, built straight from the current
	 * selection. Never stored, so there is no second copy of the selection to keep in sync. */
	FRtsActionContext MakeContext() const;

	void RefreshActions();

	UFUNCTION()
	void HandleActionClicked(UActionViewModel* ClickedAction);

	UFUNCTION()
	void HandleControlledCharacterChanged();

	UPROPERTY()
	TObjectPtr<ARtsPlayer> Player;

	/** Reused across refreshes so a rebuild that neither adds nor removes an action keeps the same
	 * row objects — the widget's list does not churn and the OnClicked binding survives. */
	UPROPERTY()
	TMap<EAction, TObjectPtr<UActionViewModel>> ActionEntryCache;
};
