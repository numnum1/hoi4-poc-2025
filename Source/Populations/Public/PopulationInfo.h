#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "PopulationInfo.generated.h"

/** Everything a pop entity needs at creation. Produced once by ScenarioLoader. */
USTRUCT(BlueprintType)
struct POPULATIONS_API FPopSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FProvinceId ProvinceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEthnicityId EthnicityId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Size = 0;
};

/**
 * One pop's full state, flattened for readers.
 *
 * Collect-style getters hand these out instead of ids so a caller that wants every pop of a province
 * gets it from one pass over the entities rather than one lookup per field.
 */
USTRUCT(BlueprintType)
struct POPULATIONS_API FPopInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FPopId Id;

	UPROPERTY(BlueprintReadOnly)
	FProvinceId ProvinceId;

	UPROPERTY(BlueprintReadOnly)
	FEthnicityId EthnicityId;

	UPROPERTY(BlueprintReadOnly)
	int32 Size = 0;
};
