// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterAction.generated.h"

class ARtsPlayerController;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ActionName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Icon;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Click(ARtsPlayerController* InPlayer, AHUD* InHUD, ARtsCharacter* InCharacter, UObject* InRoleContext);
	
};
