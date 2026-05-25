// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RaceAsset.generated.h"

/**
 * 
 */
UCLASS()
class SCENARIO_API URaceAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	URaceAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText RaceName;
};
