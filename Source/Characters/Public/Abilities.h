// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Abilities.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct CHARACTERS_API FAbilities
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Strength = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Dexterity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Constitution = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Intelligence = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Wisdom = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Charisma = 0;
};
