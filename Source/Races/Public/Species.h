// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "RaceTrait.h"
#include "Species.generated.h"

USTRUCT(BlueprintType)
struct RACES_API FSpecies
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SpeciesName = FText::FromString("Unnamed Species");

	/** Index into URaces::Categories */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CategoryIndex = -1;

	/** Index into URaces::LifeTypes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LifeTypeIndex = -1;

	/** Own traits of this species (does not include LifeType traits) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRaceTrait> Traits;
};
