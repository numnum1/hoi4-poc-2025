// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingType.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGS_API UBuildingType : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UBuildingType();
private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	FText BuildingName;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	FSlateBrush Icon;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	FName BuildingSlotName;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	int32 MaxPerProvince;
public:
	FORCEINLINE const FText& GetBuildingName() const { return BuildingName; }
	FORCEINLINE const FSlateBrush& GetIcon() const { return Icon; }
	FORCEINLINE const FName& GetBuildingSlotName() const { return BuildingSlotName; }
	FORCEINLINE int32 GetMaxPerProvince() const { return MaxPerProvince; }
};
