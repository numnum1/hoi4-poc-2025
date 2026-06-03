// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaceTrait.h"
#include "UObject/Object.h"
#include "Race.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct RACES_API FRace
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText RaceName = FText::FromString("Unnamed Race");

	/** Index into URaces::Species */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpeciesIndex = -1;

	/** Own traits of this race (does not include Species or LifeType traits) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRaceTrait> Traits = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	uint8 Strength = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	uint8 Dexterity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	uint8 Constitution = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	uint8 Intelligence = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	uint8 Wisdom = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Abilities")
	uint8 Charisma = 0;
};
