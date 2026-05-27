// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
};
