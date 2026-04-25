// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FoundableOrganisationType.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BASICCHARACTERACTIONS_API UFoundableOrganisationType : public UDataAsset
{
	GENERATED_BODY()
public:
	UFoundableOrganisationType();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TypeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UBuildable*> Buildables;
	
};
