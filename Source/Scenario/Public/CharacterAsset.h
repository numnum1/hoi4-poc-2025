// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterAsset.generated.h"

class UProvinceAsset;
class URaceAsset;

UENUM(BlueprintType)
enum class ECharacterSimTier : uint8
{
	CST_None UMETA(DisplayName="None", ToolTip="For only The Basics, No Simulation"),
	CST_Normal UMETA(DisplayName="Normal", ToolTip="For interactable characters"),
	CST_Max UMETA(DisplayName="Max", ToolTip="For active characters"),
};

/**
 * 
 */
UCLASS()
class SCENARIO_API UCharacterAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UCharacterAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	ECharacterSimTier SimTier;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText CharacterName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Portrait;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UProvinceAsset* ProvinceOfBirth;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	URaceAsset* BaseRace;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UProvinceAsset* CurrentProvince;
};
