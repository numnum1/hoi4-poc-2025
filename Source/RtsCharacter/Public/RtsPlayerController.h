// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RtsPlayerController.generated.h"

class ARtsCharacter;
/**
 * 
 */
UCLASS(Abstract)
class RTSCHARACTER_API ARtsPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FoundOrganisation(ARtsCharacter* InForCharacter);
};
