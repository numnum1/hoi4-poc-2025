// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OrganizationPosition.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ORGANISATION_API UOrganizationPosition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UOrganizationPosition();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PositionName = FText::FromString("Unnamed Position");
};
