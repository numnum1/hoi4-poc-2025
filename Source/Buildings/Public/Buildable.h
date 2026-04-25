// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Buildable.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BUILDINGS_API UBuildable : public UDataAsset
{
	GENERATED_BODY()
public:
	UBuildable();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText BuildableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Thumbnail;
};
