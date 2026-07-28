#pragma once

#include "CoreMinimal.h"
#include "ActionType.h"
#include "RtsActionContext.h"
#include "MVVMViewModelBase.h"
#include "ActionViewModel.generated.h"

/**
 * One action, shown as one row in a character's action list. Pure UI data (name, tooltip, icon)
 * plus a click callback — everything about what the action actually is comes from URtsActions,
 * switching over the EAction this entry was given. Gating (is this allowed right now, and why not)
 * and running the action live in UCharacterActionsViewModel, not here — see its
 * HandleActionClicked and GetGatedBlockedReason.
 *
 * Concrete, not abstract: there is one EAction, hardcoded in URtsActions, per action — no per-action
 * subclass. UCharacterActionsViewModel hands out a fresh instance per offer, so an entry never
 * leaks state between characters or across refreshes.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UActionViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Called by UCharacterActionsViewModel on every refresh: records which action this entry is
	 * and recomputes DisplayName/Icon from the current context. Enabled state and tooltip are not
	 * computed here — see SetEnabled. */
	void SetContext(EAction InActionType, const FRtsActionContext& InContext);

	/** Pushed by UCharacterActionsViewModel after it evaluates the gate for this action. */
	void SetEnabled(bool bInEnabled, const FText& InTooltip);

	EAction GetActionType() const { return ActionType; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionClicked, UActionViewModel*, ClickedAction);

	/** UCharacterActionsViewModel binds one handler here for every action it creates; that handler
	 * re-verifies the gate and runs the action. Bind a button's OnClicked to CallOnClicked() below,
	 * not directly to this delegate. */
	UPROPERTY(BlueprintAssignable, Category="Action")
	FOnActionClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Action")
	void CallOnClicked();

protected:
	//~ FieldNotify UI state, fed by SetContext/SetEnabled -----------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Action", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Action", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Action", meta=(AllowPrivateAccess))
	FText Tooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsEnabled, Category="Action", meta=(AllowPrivateAccess))
	bool bIsEnabled = false;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }
	FText GetTooltip() const { return Tooltip; }
	bool IsEnabled() const { return bIsEnabled; }

private:
	EAction ActionType = EAction::EA_FoundOrganization;
};
