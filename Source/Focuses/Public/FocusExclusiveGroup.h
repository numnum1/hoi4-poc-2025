#pragma once

#include "CoreMinimal.h"
#include "FocusExclusiveGroup.generated.h"

class UOrganizationFocusAsset;

USTRUCT(BlueprintType)
struct FOCUSES_API FFocusExclusiveGroup
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UOrganizationFocusAsset>> Focuses;
};
