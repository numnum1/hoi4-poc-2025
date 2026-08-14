#pragma once

#include "CoreMinimal.h"
#include "EthnicityInfo.generated.h"

/**
 * One entry in the ethnicity library — Dutch, German, Englishman. Authored once in the scenario and
 * never mutated, which is why it lives on the UPopulations facade rather than in a fragment: a pop
 * entity carries only the FEthnicityId pointing here.
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
};
