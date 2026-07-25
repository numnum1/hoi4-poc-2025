// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProvinceInfo.generated.h"

class UEcology;

/**
 * The static, client-visible half of a province.
 *
 * Everything in here is authored once in the scenario and never changes, so it is replicated
 * as one array on UProvinces and deliberately kept out of Mass — FText, FSlateBrush and the
 * two UObject pointers would all need GC handling that Mass fragments do not give them.
 * TimezoneOffset lives here rather than in a fragment because it is authored data the clients
 * need for the UI, not simulation state; each character entity caches its own copy at spawn.
 */
USTRUCT(BlueprintType)
struct PROVINCES_API FProvinceDisplayInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MapId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name = FText::FromString("Unnamed Province");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Picture = FSlateNoResource();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* InspectionSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEcology* Ecology = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimezoneOffset = 0;
};

/** Everything a province entity needs at creation. Produced once by ScenarioLoader. */
USTRUCT(BlueprintType)
struct PROVINCES_API FProvinceSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FProvinceDisplayInfo Display;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OwningNationIndex = 0;
};
