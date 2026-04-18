// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ScheduleEntry.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct SCHEDULE_API FScheduleEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name = FText::FromString("Unnamed Entry");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInt32Range> Times = {};
};
