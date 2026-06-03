// Fill out your copyright notice in the Description page of Project Settings.

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
