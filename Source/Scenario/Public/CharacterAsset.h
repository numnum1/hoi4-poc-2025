// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterSimTier.h"
#include "Engine/DataAsset.h"
#include "CharacterAsset.generated.h"

class UProvinceAsset;
class URaceAsset;

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

public:
	FORCEINLINE ECharacterSimTier GetSimTier() const
	{
		return SimTier;
	}

	FORCEINLINE const FText& GetCharacterName() const
	{
		return CharacterName;
	}

	FORCEINLINE const FSlateBrush& GetPortrait() const
	{
		return Portrait;
	}

	FORCEINLINE UProvinceAsset* GetProvinceOfBirth() const
	{
		return ProvinceOfBirth;
	}

	FORCEINLINE URaceAsset* GetBaseRace() const
	{
		return BaseRace;
	}

	FORCEINLINE UProvinceAsset* GetCurrentProvince() const
	{
		return CurrentProvince;
	}
};
