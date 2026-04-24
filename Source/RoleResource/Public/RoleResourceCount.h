// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RoleResourceCount.generated.h"

class URoleResource;

/**
 * 
 */
USTRUCT(BlueprintType)
struct ROLERESOURCE_API FRoleResourceCount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URoleResource* Resource = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;
};
