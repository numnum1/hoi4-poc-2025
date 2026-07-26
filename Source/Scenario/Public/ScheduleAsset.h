// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Schedule.h"
#include "Engine/DataAsset.h"
#include "ScheduleAsset.generated.h"

/**
 * A human-authored daily routine, referenced by the UCharacterAssets that follow it.
 *
 * ScenarioLoader collects the unique assets across all characters into the USchedules library
 * and copies the FSchedule itself onto each character's entity — the same discovered-from-
 * characters authoring pattern URaceAsset uses, so a schedule nobody follows needs no entry
 * anywhere. Without one of these a character's schedule stays all-NONE and the hourly sim
 * processor has nothing to act on.
 */
UCLASS()
class SCENARIO_API UScheduleAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UScheduleAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText ScheduleName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSchedule Schedule;
public:
	FORCEINLINE const FText& GetScheduleName() const { return ScheduleName; }
	FORCEINLINE const FSchedule& GetSchedule() const { return Schedule; }
};
