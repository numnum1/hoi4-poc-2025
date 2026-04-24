// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAction.h"
#include "UObject/Object.h"
#include "BuyPropertyAction.generated.h"

/**
 * 
 */
UCLASS()
class BASICCHARACTERACTIONS_API UBuyPropertyAction : public UCharacterAction
{
	GENERATED_BODY()
public:
	UBuyPropertyAction();
	
	virtual void Click_Implementation(UObject* InRoleContext) override;
};
