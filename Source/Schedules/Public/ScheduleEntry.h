// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ScheduleEntry.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct SCHEDULES_API FScheduleEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::FromString("Unnamed Entry");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInt32Range> Timeslots = {};
	
};
