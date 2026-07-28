#pragma once

#include "CoreMinimal.h"
#include "ActionType.h"
#include "RtsActionContext.h"
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
class RTSPROXY_API URtsActions : public UBlueprintFunctionLibrary
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

	/** An extra, action-specific check run after the shared position/permission gate passes.
	 * Empty means allowed. */
	static FText GetBlockedReason(EAction InAction, const FRtsActionContext& InContext);

	static void ExecuteAction(EAction InAction, const FRtsActionContext& InContext);
};
