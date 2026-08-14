#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "PopulationFragments.generated.h"

/** Back-reference from an entity to its stable FPopId. */
USTRUCT()
struct POPULATIONS_API FPopIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FPopId Id;
};

/**
 * Where this pop lives. A fragment rather than a tag because migration will move a pop between
 * provinces, and an archetype split per province would be one archetype per province.
 */
USTRUCT()
struct POPULATIONS_API FPopProvinceFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FProvinceId ProvinceId;
};

/**
 * Which ethnicity these people are. Never changes for a given pop — a pop assimilating into another
 * ethnicity moves head count between two pops rather than rewriting this.
 */
USTRUCT()
struct POPULATIONS_API FPopEthnicityFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FEthnicityId EthnicityId;
};

/** How many people. The only pop state that changes today; growth and migration write it. */
USTRUCT()
struct POPULATIONS_API FPopSizeFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Size = 0;
};

USTRUCT()
struct POPULATIONS_API FPopTag : public FMassTag
{
	GENERATED_BODY()
};
