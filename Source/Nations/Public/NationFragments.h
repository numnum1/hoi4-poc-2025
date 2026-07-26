#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "NationFragments.generated.h"

/**
 * Back-reference from an entity to its stable FNationId.
 * Provinces, Organizations and every view model still address nations by this id.
 */
USTRUCT()
struct NATIONS_API FNationIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FNationId Id;
};

/**
 * Minimal on purpose: nations have no mutable simulation state yet. This tag is the archetype
 * anchor future gameplay fragments (diplomacy, treasury, ...) attach next to.
 */
USTRUCT()
struct NATIONS_API FNationTag : public FMassTag
{
	GENERATED_BODY()
};
