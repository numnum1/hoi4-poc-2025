// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RaceTrait.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct RACES_API FRaceTrait
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TraitName = FText::FromString("Unnamed Trait");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description = FText::FromString("");
	
};
