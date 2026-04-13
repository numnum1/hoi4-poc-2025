// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"
#include "CharacterRole.generated.h"

class ARtsCharacter;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract, DefaultToInstanced, EditInlineNew)
class RTSCHARACTER_API UCharacterRole : public UObject
{
	GENERATED_BODY()
public:
	UCharacterRole();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintGetter=GetActions, Replicated, ReplicatedUsing=OnRep_Actions)
	TArray<class UCharacterAction*> Actions;
	
public:
	UFUNCTION(BlueprintGetter)
	const TArray<UCharacterAction*>& GetActions() const { return Actions;}
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE void AddAction(UCharacterAction* InAction)
	{
		verify(InAction);
		ensureMsgf(!Actions.Contains(InAction), TEXT("Trying to add action that already exists"));
		
		Actions.Add(InAction);
		UE_LOG(LogTemp, Warning, TEXT("Added Action"));
	}
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FORCEINLINE void RemoveAction(UCharacterAction* InAction)
	{
		ensureMsgf(InAction, TEXT("Trying to remove null action"));
		ensureMsgf(Actions.Contains(InAction), TEXT("Trying to add action that already exists"));
		
		Actions.Remove(InAction);
		UE_LOG(LogTemp, Warning, TEXT("Removed Action"));
	}
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayCharacter(ARtsCharacter* InCharacter);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool IsSupportedForNetworking() const override { return true; }

private:
	UFUNCTION()
	FORCEINLINE void OnRep_Actions()
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionsReplicated"));
	}
	
};
