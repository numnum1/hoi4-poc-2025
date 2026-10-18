#pragma once

#include "CoreMinimal.h"
#include "FocusTreeLayoutCell.generated.h"

USTRUCT(BlueprintType)
struct RTSUI_API FFocusTreeLayoutCell
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Focus Tree")
	int32 Row = 0;

	UPROPERTY(BlueprintReadOnly, Category="Focus Tree")
	float Column = 0.f;
};
