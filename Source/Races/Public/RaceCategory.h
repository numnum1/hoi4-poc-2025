// Fill out your copyright notice in the Description page of Project Settings.

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
