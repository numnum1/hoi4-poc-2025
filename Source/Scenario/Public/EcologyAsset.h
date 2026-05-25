// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EcologyAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SCENARIO_API UEcologyAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UEcologyAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText EcologyName;
};
