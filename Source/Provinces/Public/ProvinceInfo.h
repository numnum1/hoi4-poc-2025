// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProvinceInfo.generated.h"

class UEcology;
/**
 * 
 */
USTRUCT(BlueprintType)
struct PROVINCES_API FProvinceInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MapId = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name = FText::FromString("Unnamed Province");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Picture = FSlateNoResource();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* InspectionSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEcology* Ecology = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimezoneOffset = 0;

	/**
	 * This is only used on start-up and will not be looked up during the games runtime
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName OwningNationId = NAME_None;
};
