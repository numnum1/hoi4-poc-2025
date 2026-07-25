#pragma once

#include "CoreMinimal.h"
#include "Mass/EntityElementTypes.h"
#include "ProvinceFragments.generated.h"

/**
 * Back-reference from an entity to the legacy int32 province index.
 * Characters, buildings and the map widget all address provinces by index.
 */
USTRUCT()
struct PROVINCES_API FProvinceIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Index = INDEX_NONE;
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
	int32 OwningNationIndex = 0;
};

USTRUCT()
struct PROVINCES_API FProvinceTag : public FMassTag
{
	GENERATED_BODY()
};
