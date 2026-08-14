// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EthnicityAsset.h"
#include "ProvinceAsset.generated.h"

class UEcology;
class UNationAsset;
/**
 * 
 */
UCLASS()
class SCENARIO_API UProvinceAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UProvinceAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FName MapId;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText ProvinceName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Picture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UNationAsset* OwningNation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	USoundBase* InspectionSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UEcology* Ecology;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int32 TimezoneOffset;

	/** Who lives here, split by ethnicity — one entry becomes one pop entity. An ethnicity listed
	 * twice is summed rather than creating two pops for the same people. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FProvincePopulationEntry> Population;
public:
	FORCEINLINE const FName& GetMapId() const { return MapId; }
	FORCEINLINE const FText& GetProvinceName() const { return ProvinceName; }
	FORCEINLINE const FSlateBrush& GetPicture() const { return Picture; }
	FORCEINLINE UNationAsset* GetOwningNation() const { return OwningNation; }
	FORCEINLINE USoundBase* GetInspectionSound() const { return InspectionSound; }
	FORCEINLINE UEcology* GetEcology() const { return Ecology; }
	FORCEINLINE int32 GetTimezoneOffset() const { return TimezoneOffset; }
	FORCEINLINE const TArray<FProvincePopulationEntry>& GetPopulation() const { return Population; }
};
