// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterAction.generated.h"

class ARtsCharacter;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class RTSCHARACTER_API UCharacterAction : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UCharacterAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Click(ARtsCharacter* InWithCharacterContext, APlayerController* InByPlayerController);
	
};
