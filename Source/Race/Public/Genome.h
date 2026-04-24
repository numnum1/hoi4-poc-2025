// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Genome.generated.h"

class URaceTrait;
/**
 * 
 */
UCLASS(Abstract)
class RACE_API UGenome : public UDataAsset
{
	GENERATED_BODY()
public:
	UGenome();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<URaceTrait*> CollectTraits() const;
};
