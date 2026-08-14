#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EthnicityAsset.generated.h"

/**
 * One authored ethnicity — Dutch, German, Englishman.
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

public:
	FORCEINLINE const FText& GetEthnicityName() const { return EthnicityName; }
	FORCEINLINE const FColor& GetMapColor() const { return MapColor; }
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
