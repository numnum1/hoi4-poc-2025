#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RequirementCondition.h"
#include "RequirementMode.h"
#include "RequirementAsset.generated.h"

UCLASS(BlueprintType)
class FOCUSES_API URequirementAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	ERequirementMode Mode = ERequirementMode::RM_All;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FRequirementCondition> Conditions;

public:
	FORCEINLINE ERequirementMode GetMode() const { return Mode; }
	FORCEINLINE const TArray<FRequirementCondition>& GetConditions() const { return Conditions; }
};
