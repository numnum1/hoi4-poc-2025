#pragma once

#include "CoreMinimal.h"
#include "EventEffectType.h"
#include "EventEffect.generated.h"

USTRUCT(BlueprintType)
struct FOCUSES_API FEventEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEventEffectType Type = EEventEffectType::EET_ChangeOrganizationFunds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="Type == EEventEffectType::EET_ChangeOrganizationFunds", EditConditionHides))
	int32 Amount = 0;
};
