// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OrganizationAsset.generated.h"

/**
 * A human-authored organization, founded at scenario load the same way a player-triggered
 * founding would be: ScenarioLoader converts this to an FOrganizationInfo and hands it to
 * UOrganizations::Add. See UProvinceAsset / UCharacterAsset for the same authoring pattern.
 */
UCLASS()
class SCENARIO_API UOrganizationAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UOrganizationAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText OrganizationName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Logo;
public:
	FORCEINLINE const FText& GetOrganizationName() const { return OrganizationName; }
	FORCEINLINE const FSlateBrush& GetLogo() const { return Logo; }
};
