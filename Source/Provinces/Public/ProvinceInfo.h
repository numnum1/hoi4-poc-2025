// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "UObject/Object.h"
#include "ProvinceInfo.generated.h"

class UEcology;

/**
 * The static, UI-facing half of a province.
 *
 * Everything in here is authored once in the scenario and never changes, so it is kept as one
 * array on UProvinces and deliberately kept out of Mass — FText, FSlateBrush and the two
 * UObject pointers would all need GC handling that Mass fragments do not give them.
 * TimezoneOffset is NOT here: it is simulation data the hourly sim reads, not UI, so it lives
 * in FProvinceTimeFragment instead — see UProvinces::GetTimezoneOffset.
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
};

/** Everything a province entity needs at creation. Produced once by ScenarioLoader. */
USTRUCT(BlueprintType)
struct PROVINCES_API FProvinceSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FProvinceDisplayInfo Display;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNationId OwningNationId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimezoneOffset = 0;
};
