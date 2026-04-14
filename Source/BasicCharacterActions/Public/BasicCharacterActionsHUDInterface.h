// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BasicCharacterActionsHUDInterface.generated.h"

class ARtsCharacter;
// This class does not need to be modified.
UINTERFACE()
class UBasicCharacterActionsHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BASICCHARACTERACTIONS_API IBasicCharacterActionsHUDInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CreateOrganisationForm(ARtsCharacter* InCharacter);
};
