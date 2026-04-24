// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LifeType.generated.h"

class URaceTrait;
/**
 * 
 */
UCLASS(BlueprintType)
class RACE_API ULifeType : public UDataAsset
{
	GENERATED_BODY()
public:
	ULifeType();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TypeName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<URaceTrait*> Traits;
};
