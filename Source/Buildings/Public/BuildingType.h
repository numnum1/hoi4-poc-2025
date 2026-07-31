// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingType.generated.h"

/**
 * Purely the display half of a building type — name and icon. No capacity/slot data: building
 * placement is unrestricted for now (see Buildings module rework notes).
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
public:
	FORCEINLINE const FText& GetBuildingName() const { return BuildingName; }
	FORCEINLINE const FSlateBrush& GetIcon() const { return Icon; }
};
