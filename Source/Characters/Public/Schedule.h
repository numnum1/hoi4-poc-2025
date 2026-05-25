// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScheduleEntry.h"
#include "UObject/Object.h"
#include "Schedule.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct CHARACTERS_API FSchedule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FScheduleEntry> Entries = {};
	
};
