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
	
	virtual void RegisterAddedToCharacter_Implementation(ARtsCharacter* InCharacter) override;
	virtual void RegisterRemoveFromCharacter_Implementation(ARtsCharacter* InCharacter) override;
	
private:
	UFUNCTION()
	void UpdateNameAndIcon(ARtsCharacter* InCharacter);
	
};
