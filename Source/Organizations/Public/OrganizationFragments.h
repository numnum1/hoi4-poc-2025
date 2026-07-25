#pragma once

#include "CoreMinimal.h"
#include "Mass/EntityElementTypes.h"
#include "Mass/ExternalSubsystemTraits.h"   // TMassFragmentTraits
#include "OrganizationFragments.generated.h"

// -----------------------------------------------------------------------------
// Identity
// -----------------------------------------------------------------------------

/**
 * Back-reference from an entity to the legacy int32 organization index.
 * FPosition::OrganizationIndex and every view model still address organizations by index,
 * so each entity carries the index it was created for.
 */
USTRUCT()
struct ORGANIZATIONS_API FOrganizationIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Index = INDEX_NONE;
};

// -----------------------------------------------------------------------------
// Mutated by gameplay
// -----------------------------------------------------------------------------

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
	TArray<int32> Values;
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
