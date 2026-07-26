#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "ProvinceFragments.generated.h"

/**
 * Back-reference from an entity to its stable FProvinceId.
 * Characters, buildings and the map widget all address provinces by this id.
 */
USTRUCT()
struct PROVINCES_API FProvinceIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FProvinceId Id;
};

/**
 * Who owns the province. The only province state that changes during play today, which is why
 * it is the only one that became a fragment — the rest of FProvinceDisplayInfo is authored data.
 * Population, resources and stability belong next to this one when they arrive.
 */
USTRUCT()
struct PROVINCES_API FProvinceOwnershipFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FNationId OwningNationId;
};

/**
 * Authored at scenario load and never mutated, but simulation data rather than UI data — the
 * hourly character sim reads it, so it lives here, not in FProvinceDisplayInfo. Each character
 * entity still caches its own copy at spawn/move so the sim processor needs no province lookup.
 */
USTRUCT()
struct PROVINCES_API FProvinceTimeFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TimezoneOffset = 0;
};

USTRUCT()
struct PROVINCES_API FProvinceTag : public FMassTag
{
	GENERATED_BODY()
};
