// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingType.generated.h"

/**
 * One kind of building a player can put up: display data plus the two authored facts that gate
 * gameplay around it. What a building *is* lives entirely here — there is no EBuildingKind enum
 * and no switch anywhere answering "is this a headquarters" or "what does it cost"; code only
 * ever reads these fields, the same way URoles::GetLeadershipRole reads FRoleInfo::bIsLeadership
 * off a scenario-authored URoleAsset instead of switching on a role enum.
 *
 * The catalogue of which assets exist is UBuildingCatalogSettings, a project-wide setting rather
 * than per-scenario data — a building's kind does not vary between playthroughs the way a
 * character or an organization does.
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

	/** What putting one up costs the character who orders it. */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	int32 BuildCost = 0;

	/** Whether this kind can seat an organization — the found-organization modal offers exactly the
	 * building types with this set, and UBuildings::Destroy refuses to tear one down once it is an
	 * organization's seat. Same authored-flag shape as FRoleInfo::bIsLeadership. */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	bool bIsHeadquarters = false;
public:
	FORCEINLINE const FText& GetBuildingName() const { return BuildingName; }
	FORCEINLINE const FSlateBrush& GetIcon() const { return Icon; }
	FORCEINLINE int32 GetBuildCost() const { return BuildCost; }
	FORCEINLINE bool IsHeadquarters() const { return bIsHeadquarters; }
};
