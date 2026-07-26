#pragma once

#include "CoreMinimal.h"
#include "BuildingOwner.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "BuildingFragments.generated.h"

/**
 * Back-reference from an entity to its stable FBuildingId.
 * The UI facade and every view model still address buildings by this id.
 */
USTRUCT()
struct BUILDINGS_API FBuildingIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FBuildingId Id;
};

/** Set once at creation; a building does not relocate. */
USTRUCT()
struct BUILDINGS_API FBuildingLocationFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FProvinceId ProvinceId;
};

/**
 * Index into UBuildings::BuildingTypes rather than a UBuildingType* — Mass does not enter
 * TObjectPtr fields in fragments into the GC reference graph, the same reason FText/FSlateBrush
 * stay out of every other domain's fragments.
 */
USTRUCT()
struct BUILDINGS_API FBuildingTypeFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TypeIndex = INDEX_NONE;
};

USTRUCT()
struct BUILDINGS_API FBuildingOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FBuildingOwner Owner;
};

USTRUCT()
struct BUILDINGS_API FBuildingTag : public FMassTag
{
	GENERATED_BODY()
};
