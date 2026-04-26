#pragma once

#include "CoreMinimal.h"
#include "CharacterAction.h"
#include "Modules/ModuleManager.h"
#include "RoleResourceCount.h"
#include "CharacterRole.generated.h"

class UCharacterAction;

class FCharacterRoleModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterRoleEvent);

/**
 * This creates the actions
 */
UCLASS(BlueprintType, Blueprintable)
class CHARACTERROLE_API UCharacterRole : public UActorComponent
{
	GENERATED_BODY()
public:
	UCharacterRole();

	UPROPERTY(BlueprintAssignable)
	FCharacterRoleEvent OnChanged;

	UPROPERTY(BlueprintAssignable)
	FCharacterRoleEvent OnResourcesChanged;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetRoleName, Replicated, ReplicatedUsing=OnRep_Fields)
	FText RoleName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetIcon, Replicated, ReplicatedUsing=OnRep_Fields)
	FSlateBrush Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetRoleName, Replicated, ReplicatedUsing=OnRep_Fields)
	FText Title;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintGetter=GetActions, Replicated, ReplicatedUsing=OnRep_Fields)
	TArray<UCharacterAction*> Actions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetResources, Replicated, ReplicatedUsing=OnRep_Resources, BlueprintGetter=GetResources)
	TArray<FRoleResourceCount> Resources;
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintAuthorityOnly)
	void PutResource(const FRoleResourceCount& NewResourceCount);
	
	UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
	FORCEINLINE void SetRoleName(const FText& NewRoleName)
	{
		RoleName = NewRoleName;
		OnChanged.Broadcast();
	}
	
	UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
	FORCEINLINE void SetIcon(const FSlateBrush& NewIcon)
	{
		Icon = NewIcon;
		OnChanged.Broadcast();
	}
	
	UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
	FORCEINLINE void SetTitle(const FText& NewTitle)
	{
		Title = NewTitle;
		OnChanged.Broadcast();
	}
	
	UFUNCTION(BlueprintGetter)
	const TArray<UCharacterAction*>& GetActions() const { return Actions;}
	
protected:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE void AddAction(UCharacterAction* InAction)
	{
		ensureMsgf(!Actions.Contains(InAction), TEXT("Trying to add action that already exists"));
		
		Actions.Add(InAction);
		AddReplicatedSubObject(InAction);
		OnChanged.Broadcast();
	}
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE void RemoveAction(UCharacterAction* InAction)
	{
		ensureMsgf(Actions.Contains(InAction), TEXT("Trying to add action that already exists"));
		
		Actions.Remove(InAction);
		RemoveReplicatedSubObject(InAction);
		OnChanged.Broadcast();
	}
	
	UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
	FORCEINLINE void SetResources(const TArray<FRoleResourceCount>& NewResources)
	{
		Resources = NewResources;
		OnResourcesChanged.Broadcast();
	}
	
	UFUNCTION(BlueprintGetter)
	const TArray<FRoleResourceCount>& GetResources() const { return Resources; }
	
private:
	UFUNCTION()
	FORCEINLINE void OnRep_Fields()
	{
		OnChanged.Broadcast();
	}
	
	UFUNCTION()
	FORCEINLINE void OnRep_Resources()
	{
		OnResourcesChanged.Broadcast();
	}
	
};