// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BankAsset.h"
#include "CharacterSimTier.h"
#include "Engine/DataAsset.h"
#include "CharacterAsset.generated.h"

class UProvinceAsset;
class URaceAsset;
class UScheduleAsset;

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

	/** The daily routine this character follows. Without one it does nothing all day. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UScheduleAsset* Schedule;

	/** Which bank accounts to open at scenario load — a character can hold several, possibly at
	 * different banks. An entry with Bank == nullptr is skipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FBankAccountAsset> BankAccounts;

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

	FORCEINLINE UScheduleAsset* GetSchedule() const
	{
		return Schedule;
	}

	FORCEINLINE const TArray<FBankAccountAsset>& GetBankAccounts() const
	{
		return BankAccounts;
	}
};
