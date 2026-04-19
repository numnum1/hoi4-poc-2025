// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RacialLevel.generated.h"

class URace;
/**
 * 
 */
USTRUCT(BlueprintType)
struct CHARACTERBUILD_API FRacialLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	URace* Race = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	int32 Level = 0;
};
