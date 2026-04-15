// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterRole.h"
#include "SelfRole.generated.h"


UCLASS(BlueprintType)
class RTSCHARACTER_API USelfRole : public UCharacterRole
{
	GENERATED_BODY()

public:
	USelfRole();
	
	virtual void SetupWithContext_Implementation(AActor* InContext) override;
	
private:
	
	void RegisterAddedToCharacter(class ARtsCharacter* InCharacter);
	UFUNCTION()
	void RegisterRemoveFromCharacter(AActor* InCharacter);
	
	UFUNCTION()
	void UpdateNameAndIcon(ARtsCharacter* InCharacter);
	
};
