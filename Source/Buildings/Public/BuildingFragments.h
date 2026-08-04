#pragma once

#include "CoreMinimal.h"
#include "BuildingKind.h"
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
 * What kind of building this is. A plain enum, so the entity carries the whole answer — there is
 * no registry to resolve it through and nothing here that Mass would have to enter into the GC
 * reference graph. UBuildingKinds turns it into display data, a build cost and the headquarters
 * rule.
 */
USTRUCT()
struct BUILDINGS_API FBuildingKindFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	EBuildingKind Kind = EBuildingKind::BK_None;
};

/**
 * Presence, not a stored enum, is the owner type — exactly one of this and
 * FBuildingOrganizationOwnerFragment is ever added to a building entity. See
 * UBuildingEntitySubsystem::SetCharacterOwner/SetOrganizationOwner for the archetype move that
 * keeps that true.
 */
USTRUCT()
struct BUILDINGS_API FBuildingCharacterOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FCharacterId OwnerId;
};

USTRUCT()
struct BUILDINGS_API FBuildingOrganizationOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FOrganizationId OwnerId;
};

USTRUCT()
struct BUILDINGS_API FBuildingTag : public FMassTag
{
	GENERATED_BODY()
};
