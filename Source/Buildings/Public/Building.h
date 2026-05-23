// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingType.h"
#include "UObject/Object.h"
#include "Building.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BUILDINGS_API FBuilding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBuildingType* Type = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Owner = NAME_None;
};
