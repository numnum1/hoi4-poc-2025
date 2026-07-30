#pragma once

#include "CoreMinimal.h"
#include "ActionType.h"
#include "RtsActionContext.h"
#include "MVVMViewModelBase.h"
#include "ActionViewModel.generated.h"

/**
 * One action, shown as one row in a character's action list. Pure UI data (name, tooltip, icon)
 * plus a click callback, and nothing else — what the action *is* comes from URtsActions, switching
 * over the EAction this row was given, and whether it is allowed right now is decided by the owning
 * view model (UPlayerCharacterViewModel), which is the one that knows the current selection.
 *
 * Concrete, not abstract: there is one hardcoded EAction per action, no per-action subclass.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UActionViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Records which action this row stands for and pulls its display data out of URtsActions.
	 * Enabled state and tooltip are not derived here — see SetEnabled. */
	void SetAction(EAction InActionType);

	/** Pushed by the owning view model after it evaluates the gate for this action. */
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
