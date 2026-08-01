#pragma once

#include "CoreMinimal.h"
#include "ActionType.h"
#include "RtsActionContext.h"
#include "RtsActionInput.h"
#include "Characters.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsActions.generated.h"

/**
 * The action catalogue, hardcoded as a set of small switches over EAction rather than a lookup
 * table or a UObject subclass per action. GetActionsForSelf/GetActionsForPosition decide which
 * actions apply to a scope; the rest answer everything UCharacterActionsViewModel needs to gate,
 * display and run one action. Adding an action means adding one EAction entry and one case to each
 * switch in RtsActions.cpp — nothing else.
 *
 * A UBlueprintFunctionLibrary rather than a plain namespace, the same shape UScheduleStatics uses,
 * so a widget can call the pure queries directly if it ever needs to (building a scope picker, say)
 * instead of only through UCharacterActionsViewModel.
 */
UCLASS()
class RTSVIEWMODELS_API URtsActions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Actions available while acting as yourself. */
	UFUNCTION(BlueprintPure, Category="Rts Actions")
	static TArray<EAction> GetActionsForSelf();

	/** Actions available while acting as the given position. */
	UFUNCTION(BlueprintPure, Category="Rts Actions")
	static TArray<EAction> GetActionsForPosition(FPosition InPosition);

	static ERtsActionScope GetScope(EAction InAction);
	static FText GetDisplayName(EAction InAction);
	static FSlateBrush GetIcon(EAction InAction);

	/** What a RAS_Position context's role must grant. Ignored for RAS_Self actions. */
	static int32 GetRequiredPermissions(EAction InAction);

	/**
	 * The whole gate for one action in one context — empty means allowed, otherwise the reason to
	 * show. Verifies that the action is genuinely offered for this scope and, for RAS_Position, that
	 * the character actually holds the claimed (organization, role) pair and that the role grants
	 * GetRequiredPermissions. A context is a claim, not a fact: this re-derives everything from the
	 * character itself.
	 *
	 * A pure function of (action, context) — the context carries the facades this needs, so it
	 * never resolves a subsystem itself — which is why it lives here and not on the view model
	 * that happens to hold the current selection.
	 */
	static FText GetBlockedReason(EAction InAction, const FRtsActionContext& InContext);

	/**
	 * The gate for the values a modal collected — empty means the input is usable. Separate from
	 * GetBlockedReason because the two answer different questions at different times: whether the
	 * action may be offered at all, versus whether what the player typed is acceptable. A widget
	 * re-runs this on every keystroke; both run again on confirm.
	 *
	 * Returns empty for actions that take no input.
	 */
	static FText GetInvalidInputReason(EAction InAction, const FRtsActionContext& InContext, const FRtsActionInput& InInput);

	static void ExecuteAction(EAction InAction, const FRtsActionContext& InContext, const FRtsActionInput& InInput);
};
