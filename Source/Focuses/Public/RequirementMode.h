#pragma once

#include "CoreMinimal.h"
#include "RequirementMode.generated.h"

UENUM(BlueprintType)
enum class ERequirementMode : uint8
{
	RM_All UMETA(DisplayName = "All"),
	RM_Any UMETA(DisplayName = "Any"),
};
