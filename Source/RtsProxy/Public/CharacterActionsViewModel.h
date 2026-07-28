#pragma once

#include "CoreMinimal.h"
#include "ActionType.h"
#include "RtsActionContext.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "CharacterActionsViewModel.generated.h"

class UActionViewModel;
class UCharacterProxy;
class UOrganizations;

/**
 * The action list offered to one character in one scope — "found an organization" while acting as
 * themselves, "publish alignment" while acting as a position at an organization.
 *
 * Call Initialize once with the character to display, the same single-parameter shape every other
 * view model in this module uses. The scope is then switched with SetScopeSelf/SetScopePosition;
 * the position variant takes an index into the character's own positions and resolves the
 * organization and role from it, so the two can never disagree and the caller cannot claim a
 * position the character does not hold. Every UActionViewModel re-verifies that claim itself
 * before it reports itself enabled, so this view model does not have to be trusted either.
 *
 * Rebuilds on two triggers: the character's own UCharacterProxy::OnChanged (which covers a new
 * position via OnPositionChanged) and UOrganizations::OnOrganizationAdded, since a freshly founded
 * organization immediately becomes a position the player can act as. Action view models are cached
 * per class and repointed via SetContext rather than reallocated, so a refresh that neither adds
 * nor removes an action still updates each row's enabled state and blocked reason.
 *
 * UActionViewModel itself holds only UI data (name, tooltip, icon) and an OnClicked callback — what
 * an EAction actually is (its scope, permissions, display data, what running it does) comes from
 * URtsActions; the gate (is this action allowed right now) and running it live here, since this
 * class is the one that actually knows the acting character and the current scope.
 * GetGatedBlockedReason computes the gate for one entry; Refresh_Implementation pushes the result
 * into that entry via SetEnabled, and HandleActionClicked re-runs the same gate at click time
 * before calling URtsActions::ExecuteAction — never trusts whatever was last pushed into the entry.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSPROXY_API UCharacterActionsViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Public, unlike the other view models in this module: an owning object (not a subclass —
	 * see UPlayerCharacterViewModel in RtsUI) needs to drive this one directly. */
	UFUNCTION(BlueprintCallable, Category="Character Actions Viewmodel")
	void Initialize(FCharacterId InCharacterId);

	/** Act as the character themselves. */
	UFUNCTION(BlueprintCallable, Category="Character Actions Viewmodel")
	void SetScopeSelf();

	/** Act as one of the character's positions, indexed as UCharacterProxy::GetOrganization does. */
	UFUNCTION(BlueprintCallable, Category="Character Actions Viewmodel")
	void SetScopePosition(int32 InPositionIndex);

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Actions Viewmodel", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UActionViewModel>> Actions;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Actions Viewmodel", meta=(AllowPrivateAccess))
	int32 ActionCount = 0;

public:
	const TArray<TObjectPtr<UActionViewModel>>& GetActions() const { return Actions; }
	int32 GetActionCount() const { return ActionCount; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Character Actions Viewmodel")
	void Refresh();
	virtual void Refresh_Implementation();

private:
	void Unbind();

	UFUNCTION()
	void HandleProxyChanged();

	UFUNCTION()
	void HandleOrganizationAdded();

	UFUNCTION()
	void HandleActionClicked(UActionViewModel* ClickedAction);

	/** Not just for Refresh: HandleActionClicked calls this again at click time rather than
	 * trusting whichever bIsEnabled the entry last had pushed into it. */
	FText GetGatedBlockedReason(const UActionViewModel* Action) const;
	bool HoldsClaimedPosition() const;

	virtual void BeginDestroy() override;

	UPROPERTY()
	TObjectPtr<UCharacterProxy> Proxy;

	/** Cached so Unbind, called from BeginDestroy, never has to re-resolve the proxy subsystem. */
	UPROPERTY()
	TObjectPtr<UOrganizations> Organizations;

	FCharacterId CharacterId;

	UPROPERTY()
	FRtsActionContext Context;

	UPROPERTY()
	TMap<EAction, TObjectPtr<UActionViewModel>> EntryCache;
};
