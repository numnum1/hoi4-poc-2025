// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JobLevel.generated.h"

class UJob;
/**
 * 
 */
USTRUCT(BlueprintType)
struct CHARACTERBUILD_API FJobLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UJob* Race = nullptr;
	
	// Can level faster
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FocusPoints = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EXP = 0;
	
};
