// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAction.h"
#include "UObject/Object.h"
#include "FoundOrganisationAction.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BASICCHARACTERACTIONS_API UFoundOrganisationAction : public UCharacterAction
{
	GENERATED_BODY()
public:
	UFoundOrganisationAction();
	
	virtual void Click_Implementation(ARtsPlayerController* InPlayer, AHUD* InHUD, ARtsCharacter* InCharacter, UObject* InRoleContext) override;
};
