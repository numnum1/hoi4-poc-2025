#pragma once

#include "CoreMinimal.h"
#include "EventEffect.h"
#include "EventOption.generated.h"

USTRUCT(BlueprintType)
struct FOCUSES_API FEventOption
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Text;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Tooltip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FEventEffect> Effects;
};
