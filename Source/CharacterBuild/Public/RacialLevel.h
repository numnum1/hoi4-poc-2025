// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RacialLevel.generated.h"

class UGenome;
/**
 * 
 */
USTRUCT(BlueprintType)
struct CHARACTERBUILD_API FRacialLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1, ClampMax=10))
	UGenome* Genome = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1, ClampMax=10))
	int32 Level = 1;
};
