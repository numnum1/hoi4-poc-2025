#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "Mass/ExternalSubsystemTraits.h"   // TMassFragmentTraits
#include "OrganizationFragments.generated.h"

// -----------------------------------------------------------------------------
// Identity
// -----------------------------------------------------------------------------

/**
 * Back-reference from an entity to its stable FOrganizationId.
 * FPosition::OrganizationId and every view model still address organizations by this id,
 * so each entity carries the id it was created for.
 */
USTRUCT()
struct ORGANIZATIONS_API FOrganizationIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FOrganizationId Id;
};

// -----------------------------------------------------------------------------
// Mutated by gameplay
// -----------------------------------------------------------------------------

/**
 * Who owns the organization — the character who founded it, until ownership is handed on.
 *
 * The owner's leadership position (FPosition on the character) and this fragment answer two
 * different questions: the position says what the character may do inside the organization, this
 * says which single character the organization belongs to. Deriving one from the other would mean
 * scanning every character for a leadership role, and would break the moment two characters hold
 * one. An organization loaded from a scenario has no founder and keeps an invalid id.
 */
USTRUCT()
struct ORGANIZATIONS_API FOrganizationOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FCharacterId OwnerId;
};

/**
 * The ideological stance. Whether it has been published is the FOrganizationPublishedTag,
 * not a bool in here — publishing is a one-way state change worth its own archetype.
 */
USTRUCT()
struct ORGANIZATIONS_API FOrganizationAlignmentFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Branches;
};

USTRUCT()
struct ORGANIZATIONS_API FOrganizationSettledNationsFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FNationId> Values;
};

/**
 * The building the organization is seated in — and, with it, the only thing that says what kind of
 * organization this is. There is no organization type: an organization is a party because its
 * headquarters is a party office, which is a fact about the world rather than a label somebody
 * picked. See URtsActions::EA_FoundOrganization for how the two are created together.
 *
 * An FBuildingId, not an FMassEntityHandle — the id is the reference that survives a session, and
 * holding it here costs Organizations no dependency on Buildings (both ids live in RtsCore).
 * Invalid for a scenario-authored organization, which is founded without a building.
 */
USTRUCT()
struct ORGANIZATIONS_API FOrganizationHeadquartersFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FBuildingId BuildingId;
};

// Both hold open-ended arrays, so neither is a flat POD. Mass wants that said out loud.
template<>
struct TMassFragmentTraits<FOrganizationAlignmentFragment> final
{
	enum
	{
		AuthorAcceptsItsNotTriviallyCopyable = true
	};
};

template<>
struct TMassFragmentTraits<FOrganizationSettledNationsFragment> final
{
	enum
	{
		AuthorAcceptsItsNotTriviallyCopyable = true
	};
};

// -----------------------------------------------------------------------------
// Tags
// -----------------------------------------------------------------------------

USTRUCT()
struct ORGANIZATIONS_API FOrganizationTag : public FMassTag
{
	GENERATED_BODY()
};

/** Added once PublishAlignment ran. Absence means the stance is still private. */
USTRUCT()
struct ORGANIZATIONS_API FOrganizationPublishedTag : public FMassTag
{
	GENERATED_BODY()
};
