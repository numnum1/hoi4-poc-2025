// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NationAsset.generated.h"

class UProvinceAsset;
/**
 * 
 */
UCLASS(BlueprintType)
class SCENARIO_API UNationAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UNationAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText NationName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Flag;
};
