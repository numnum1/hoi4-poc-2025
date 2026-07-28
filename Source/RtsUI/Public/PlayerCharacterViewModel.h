#pragma once

#include "CoreMinimal.h"
#include "CharacterViewModel.h"
#include "PlayerCharacterViewModel.generated.h"

class ARtsPlayer;
class UCharacterActionsViewModel;

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

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TObjectPtr<UCharacterActionsViewModel> Actions;

public:
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
	bool IsActingAsSelf() const { return bIsActingAsSelf; }
	UCharacterActionsViewModel* GetActions() const { return Actions; }

protected:
	virtual void Refresh_Implementation() override;
	virtual void BeginDestroy() override;

private:
	void ApplySelection();

	UFUNCTION()
	void HandleControlledCharacterChanged();

	UPROPERTY()
	TObjectPtr<ARtsPlayer> Player;
};
