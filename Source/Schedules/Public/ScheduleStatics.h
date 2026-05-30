// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Schedule.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ScheduleStatics.generated.h"

/**
 * 
 */
UCLASS()
class SCHEDULES_API UScheduleStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE EScheduleEntryType GetEntryAtHour(const FSchedule& InTarget, const uint8 InHour)
	{
		return InTarget[InHour];
	}
};
