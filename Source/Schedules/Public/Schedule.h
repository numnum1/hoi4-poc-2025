// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScheduleEntry.h"
#include "UObject/Object.h"
#include "Schedule.generated.h"

UENUM(BlueprintType)
enum class EScheduleEntryType : uint8
{
	SET_NONE UMETA(DisplayName = "None"),
	SET_SLEEP UMETA(DisplayName = "Sleep"),
	SET_WORK UMETA(DisplayName = "Work"),
	SET_LEISURE UMETA(DisplayName = "Leisure"),
	SET_STUDY UMETA(DisplayName = "Study"),
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct SCHEDULES_API FSchedule
{
	GENERATED_BODY()
	
	TStaticArray<EScheduleEntryType, 24> Entries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Zero;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType One;
	
};
