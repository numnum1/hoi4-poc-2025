// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OrganizationInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct ORGANIZATIONS_API FOrganizationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText OrganizationName = FText::FromString("Unnamed Organization");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Logo = FSlateNoResource();
};
