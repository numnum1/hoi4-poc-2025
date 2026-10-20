#pragma once

#include "CoreMinimal.h"
#include "FocusTreeExclusiveLink.generated.h"

USTRUCT(BlueprintType)
struct RTSUI_API FFocusTreeExclusiveLink
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Focus Tree")
	int32 FirstIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Focus Tree")
	int32 SecondIndex = INDEX_NONE;
};
