// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingFunction.h"
#include "Engine/DataAsset.h"
#include "BuildingType.generated.h"

/**
 * One function this kind of building offers, and how much department space it offers for it.
 * A building hosts as many departments as fit: the sum of the space the departments seated here
 * claim for a function may not exceed the Space authored for that function.
 */
USTRUCT(BlueprintType)
struct BUILDINGS_API FBuildingFunctionSlot
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EBuildingFunction Function = EBuildingFunction::BF_Bureaucracy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Space = 0;
};

/**
 * One kind of building a player can put up: display data plus the authored facts that gate
 * gameplay around it. What a building *is* lives entirely here — there is no EBuildingKind enum
 * and no switch anywhere answering "is this a headquarters" or "what does it cost"; code only
 * ever reads these fields, the same way URoles::GetLeadershipRole reads FRoleInfo::bIsLeadership
 * off a scenario-authored URoleAsset instead of switching on a role enum.
 *
 * The functions authored here are what makes a building useful to an organization at all — a
 * department can only be seated where its function is offered. Note the direction: the building
 * offers capacity, it does not name a department type. See EBuildingFunction.
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

	/** What departments seated here may do, and how much room there is for each. A building type
	 * with an empty list can be put up but cannot seat anything. */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess), EditDefaultsOnly)
	TArray<FBuildingFunctionSlot> Functions;
public:
	FORCEINLINE const FText& GetBuildingName() const { return BuildingName; }
	FORCEINLINE const FSlateBrush& GetIcon() const { return Icon; }
	FORCEINLINE int32 GetBuildCost() const { return BuildCost; }
	FORCEINLINE bool IsHeadquarters() const { return bIsHeadquarters; }
	FORCEINLINE const TArray<FBuildingFunctionSlot>& GetFunctions() const { return Functions; }

	/** How much department space this kind offers for InFunction, or 0 if it does not offer it. */
	int32 GetSpaceFor(EBuildingFunction InFunction) const;

	/** The first authored function — what founding an organization here makes its root department
	 * do. False if nothing is authored, which is what blocks founding in that building. */
	bool GetPrimaryFunction(EBuildingFunction& OutFunction) const;
};
