// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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

	/** This province's pixel color in the world map's province-ID texture — red and green only, the
	 * two channels the map shader indexes with. Copied straight from the province row authored for
	 * the map, so nothing at runtime needs that DataTable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FColor MapPixelColor = FColor::Black;

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
public:
	FORCEINLINE const FName& GetMapId() const { return MapId; }
	FORCEINLINE const FColor& GetMapPixelColor() const { return MapPixelColor; }
	FORCEINLINE const FText& GetProvinceName() const { return ProvinceName; }
	FORCEINLINE const FSlateBrush& GetPicture() const { return Picture; }
	FORCEINLINE UNationAsset* GetOwningNation() const { return OwningNation; }
	FORCEINLINE USoundBase* GetInspectionSound() const { return InspectionSound; }
	FORCEINLINE UEcology* GetEcology() const { return Ecology; }
	FORCEINLINE int32 GetTimezoneOffset() const { return TimezoneOffset; }
};
