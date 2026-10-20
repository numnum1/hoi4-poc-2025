// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "RaceCategory.generated.h"

USTRUCT(BlueprintType)
struct RACES_API FRaceCategory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CategoryName = FText::FromString("Unnamed Category");
};
