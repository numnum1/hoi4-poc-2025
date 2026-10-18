#pragma once

#include "CoreMinimal.h"
#include "FocusTreeEdge.generated.h"

USTRUCT(BlueprintType)
struct RTSUI_API FFocusTreeEdge
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Focus Tree")
	int32 FromIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Focus Tree")
	int32 ToIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Focus Tree")
	bool bIsAlternative = false;
};
