// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingType.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGS_API UBuildingType : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UBuildingType();
private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	FText BuildingName;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	FSlateBrush Icon;	
};
