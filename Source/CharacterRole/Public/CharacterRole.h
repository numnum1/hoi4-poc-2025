#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "CharacterRole.generated.h"

class FCharacterRoleModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterRoleEvent);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract, DefaultToInstanced, EditInlineNew)
class CHARACTERROLE_API UCharacterRole : public UObject
{
	GENERATED_BODY()
public:
	UCharacterRole();

	UPROPERTY(BlueprintAssignable)
	FCharacterRoleEvent OnChanged;
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetRoleName, Replicated, ReplicatedUsing=OnRep_Fields)
	FText RoleName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetIcon, Replicated, ReplicatedUsing=OnRep_Fields)
	FSlateBrush Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetRoleName, Replicated, ReplicatedUsing=OnRep_Fields)
	FText Title;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintGetter=GetActions, Replicated, ReplicatedUsing=OnRep_Fields)
	TArray<class UCharacterAction*> Actions;
	
public:
	
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
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE void AddAction(UCharacterAction* InAction)
	{
		verify(InAction);
		ensureMsgf(!Actions.Contains(InAction), TEXT("Trying to add action that already exists"));
		
		Actions.Add(InAction);
		OnChanged.Broadcast();
	}
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE void RemoveAction(UCharacterAction* InAction)
	{
		ensureMsgf(InAction, TEXT("Trying to remove null action"));
		ensureMsgf(Actions.Contains(InAction), TEXT("Trying to add action that already exists"));
		
		Actions.Remove(InAction);
		OnChanged.Broadcast();
	}
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool IsSupportedForNetworking() const override { return true; }

private:
	UFUNCTION()
	FORCEINLINE void OnRep_Fields()
	{
		OnChanged.Broadcast();
	}
	
};