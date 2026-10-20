// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "RaceTrait.h"
#include "LifeType.generated.h"

USTRUCT(BlueprintType)
struct RACES_API FLifeType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText LifeTypeName = FText::FromString("Unnamed Life Type");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRaceTrait> Traits;
};
