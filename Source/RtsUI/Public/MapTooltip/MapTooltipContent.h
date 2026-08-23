#pragma once

#include "CoreMinimal.h"
#include "MapTooltipContent.generated.h"

/** What the map tooltip shows for whatever is currently hovered. */
USTRUCT(BlueprintType)
struct FMapTooltipContent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Map Tooltip")
	FText Headline;

	UPROPERTY(BlueprintReadWrite, Category="Map Tooltip")
	FText Content;
};
