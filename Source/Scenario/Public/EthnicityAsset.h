#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EthnicityAsset.generated.h"

class URaceAsset;

/**
 * One authored ethnicity — Dutch, German, Englishman.
 *
 * An ethnicity is a cultural subdivision of a race: many ethnicities share one race, and the race
 * is what carries the biology (species, life type, ability modifiers). Dutch and German are both
 * Human; a pop's abilities come from Race, its map color and identity from here.
 *
 * Data only, never behavior: what an ethnicity *does* (assimilation, unrest, migration pull) is
 * code, not an asset. The scenario discovers these through the province assets that place people of
 * them, the same way it discovers races through the characters that have one, so there is no
 * separate list to keep in sync.
 */
UCLASS()
class SCENARIO_API UEthnicityAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UEthnicityAsset();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText EthnicityName;

	/** What provinces dominated by this ethnicity show in the map's ethnicity mode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FColor MapColor;

	/** The race these people are. Leaving it unset costs the pops their biology, so every authored
	 * ethnicity should name one. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<URaceAsset> Race;

	/** Net growth per year (births minus deaths), as a fraction — 0.02 means 2%/year. Negative
	 * values are a legal decline. No separate death rate in this first pass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, ClampMin="-1.0"))
	float AnnualBirthRate = 0.f;

public:
	FORCEINLINE const FText& GetEthnicityName() const { return EthnicityName; }
	FORCEINLINE const FColor& GetMapColor() const { return MapColor; }
	FORCEINLINE URaceAsset* GetRace() const { return Race; }
	FORCEINLINE float GetAnnualBirthRate() const { return AnnualBirthRate; }
};

/** One ethnicity's share of a province's people, authored on the province. */
USTRUCT(BlueprintType)
struct SCENARIO_API FProvincePopulationEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEthnicityAsset> Ethnicity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	int32 Size = 0;
};
