#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "EthnicityInfo.generated.h"

/**
 * One entry in the ethnicity library — Dutch, German, Englishman. Authored once in the scenario and
 * never mutated, which is why it lives on the UPopulations facade rather than in a fragment: a pop
 * entity carries only the FEthnicityId pointing here.
 *
 * An ethnicity is a cultural subdivision of a race, so the race is named here and not on the pop:
 * every pop of an ethnicity has the same race by definition, and duplicating it per entity would
 * only create a second place for it to be wrong.
 */
USTRUCT(BlueprintType)
struct POPULATIONS_API FEthnicityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EthnicityName = FText::FromString("Unnamed Ethnicity");

	/** What this ethnicity looks like on the world map's ethnicity mode. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor MapColor = FColor::White;

	/** Into URaces' library — the biology behind the culture. Invalid if the asset named no race. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRaceId RaceId;

	/** Net growth per year (births minus deaths) as a fraction — 0.02 means 2%/year. Shared by
	 * every pop of this ethnicity via FPopGrowthSharedFragment. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnnualBirthRate = 0.f;
};
