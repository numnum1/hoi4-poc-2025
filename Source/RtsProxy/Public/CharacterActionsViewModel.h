#pragma once

#include "CoreMinimal.h"
#include "RtsActionContext.h"
#include "RtsIds.h"
#include "Templates/SubclassOf.h"
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
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSPROXY_API UCharacterActionsViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
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
	UFUNCTION(BlueprintCallable, Category="Character Actions Viewmodel")
	void Initialize(FCharacterId InCharacterId);

	UFUNCTION(BlueprintNativeEvent, Category="Character Actions Viewmodel")
	void Refresh();
	virtual void Refresh_Implementation();

private:
	void Unbind();

	UFUNCTION()
	void HandleProxyChanged();

	UFUNCTION()
	void HandleOrganizationAdded();

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
	TMap<TSubclassOf<UActionViewModel>, TObjectPtr<UActionViewModel>> EntryCache;
};
