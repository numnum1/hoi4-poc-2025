// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RaceTrait.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class RACE_API URaceTrait : public UDataAsset
{
	GENERATED_BODY()
public:
	URaceTrait();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TraitName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};
