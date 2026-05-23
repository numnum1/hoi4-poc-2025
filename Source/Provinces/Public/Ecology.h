// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Ecology.generated.h"

/**
 * 
 */
UCLASS()
class PROVINCES_API UEcology : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UEcology();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText EcologyName;
};
