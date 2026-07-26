#pragma once

#include "CoreMinimal.h"
#include "RtsActionContext.h"
#include "MVVMViewModelBase.h"
#include "ActionViewModel.generated.h"

/**
 * Base for one action, shown as one row in a character's action list.
 *
 * Every concrete action is a plain C++ subclass — see UFoundOrganizationActionViewModel —
 * implementing GetScope/GetDisplayNameFor/ExecuteAction as ordinary virtual methods. There is no
 * lookup table and no shared action object: RtsActions::CollectClassesForScope hands out a fresh
 * instance per offer, so a subclass is free to hold whatever member state it wants without it
 * leaking between characters or across refreshes.
 *
 * SetContext runs the shared gate before anything else: that the action is genuinely offered for
 * this scope, and for RAS_Position, that the character actually holds the claimed (organization,
 * role) pair and that the role grants GetRequiredPermissions(). That gate is not virtual, so a
 * subclass cannot skip it — GetActionBlockedReason is the hook for an additional, action-specific
 * check layered on top.
 */
UCLASS(Abstract, BlueprintType)
class RTSPROXY_API UActionViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Called by the owning UCharacterActionsViewModel right after construction, and on every refresh. */
	void SetContext(const FRtsActionContext& InContext);

	/** Re-verifies the gate at call time — never trusts whatever the UI last cached — and runs the action. */
	UFUNCTION(BlueprintCallable, Category="Action")
	bool Execute();

protected:
	//~ Subclass contract, plain C++ — this is where an action's behavior actually lives ----------
	// PURE_VIRTUAL rather than "= 0": Unreal still constructs a CDO for every UCLASS, abstract or
	// not, and a true C++ pure virtual would make that construction ill-formed.

	virtual ERtsActionScope GetScope() const PURE_VIRTUAL(UActionViewModel::GetScope, return ERtsActionScope::RAS_Self;);
	virtual FText GetDisplayNameFor(const FRtsActionContext& InContext) const PURE_VIRTUAL(UActionViewModel::GetDisplayNameFor, return FText::GetEmpty(););
	virtual void ExecuteAction(const FRtsActionContext& InContext) const PURE_VIRTUAL(UActionViewModel::ExecuteAction, );

	/** What a RAS_Position context's role must grant. Ignored for RAS_Self. */
	virtual int32 GetRequiredPermissions() const { return 0; }

	/** An extra, action-specific check run after the shared gate passes. Default: always allowed. */
	virtual FText GetActionBlockedReason(const FRtsActionContext& InContext) const { return FText::GetEmpty(); }

	//~ FieldNotify state, fed by SetContext -----------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Action", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsEnabled, Category="Action", meta=(AllowPrivateAccess))
	bool bIsEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Action", meta=(AllowPrivateAccess))
	FText BlockedReason;

public:
	FText GetDisplayName() const { return DisplayName; }
	bool IsEnabled() const { return bIsEnabled; }
	FText GetBlockedReason() const { return BlockedReason; }

private:
	/** Not virtual on purpose — see the class comment. */
	FText GetGatedBlockedReason(const FRtsActionContext& InContext) const;
	bool HoldsClaimedPosition(const FRtsActionContext& InContext) const;

	UPROPERTY()
	FRtsActionContext Context;
};
