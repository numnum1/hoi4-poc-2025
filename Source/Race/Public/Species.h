// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Genome.h"
#include "Engine/DataAsset.h"
#include "Species.generated.h"

class ULifeType;
class URaceTrait;
class URaceCategory;
/**
 * 
 */
UCLASS()
class RACE_API USpecies : public UGenome
{
	GENERATED_BODY()
public:
	USpecies();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URaceCategory* Category;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULifeType* Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SpeciesName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<URaceTrait*> Traits;
	
	FORCEINLINE virtual TArray<URaceTrait*> CollectTraits_Implementation() const override;
	
};
