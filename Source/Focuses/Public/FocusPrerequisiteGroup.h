#pragma once

#include "CoreMinimal.h"
#include "FocusPrerequisiteGroup.generated.h"

class UOrganizationFocusAsset;

USTRUCT(BlueprintType)
struct FOCUSES_API FFocusPrerequisiteGroup
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UOrganizationFocusAsset>> AnyOf;
};
