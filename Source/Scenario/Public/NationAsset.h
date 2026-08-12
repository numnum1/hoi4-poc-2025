// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NationAsset.generated.h"

class UProvinceAsset;
/**
 * 
 */
UCLASS(BlueprintType)
class SCENARIO_API UNationAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UNationAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText NationName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Flag;
	/** The nation's color on the world map — the map's default shading paints every province this
	 * nation owns with it, see ARtsWorldMap::ResolveVisuals. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FColor MapColor = FColor::White;
public:
	FORCEINLINE const FText& GetNationName() const { return NationName; }
	FORCEINLINE const FSlateBrush& GetFlag() const { return Flag; }
	FORCEINLINE FColor GetMapColor() const { return MapColor; }
};
