// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayCharacter(ARtsCharacter* InCharacter);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	
};
