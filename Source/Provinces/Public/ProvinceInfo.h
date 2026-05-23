// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProvinceInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROVINCES_API FProvinceInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name = FText::FromString("Province");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Picture = FSlateNoResource();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* InspectionSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EcologyName = FText::FromString("");
};
