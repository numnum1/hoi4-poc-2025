// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuildingOwner.generated.h"

UENUM(BlueprintType)
enum class EBuildingOwnerType : uint8
{
	BOT_Character UMETA(DisplayName = "Character"),
	BOT_Organization UMETA(DisplayName = "Organization")
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct BUILDINGS_API FBuildingOwner
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuildingOwnerType OwnerType = EBuildingOwnerType::BOT_Character;
};
