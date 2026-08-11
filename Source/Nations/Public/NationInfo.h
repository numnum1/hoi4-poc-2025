// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NationInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct NATIONS_API FNationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NationName = FText::FromString("Unnamed Nation");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Flag = FSlateNoResource();

	/** The nation's color on the world map — the map's default shading reads it from here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor MapColor = FColor::White;
};
