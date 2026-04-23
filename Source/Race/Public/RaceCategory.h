// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RaceCategory.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class RACE_API URaceCategory : public UDataAsset
{
	GENERATED_BODY()
public:
	URaceCategory();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CategoryName;
};
