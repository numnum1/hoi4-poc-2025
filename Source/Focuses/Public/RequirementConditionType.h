#pragma once

#include "CoreMinimal.h"
#include "RequirementConditionType.generated.h"

UENUM(BlueprintType)
enum class ERequirementConditionType : uint8
{
	RCT_Nested                UMETA(DisplayName = "Nested Requirement"),
	RCT_MinOrganizationFunds  UMETA(DisplayName = "Min Organization Funds"),
	RCT_HasDepartmentFunction UMETA(DisplayName = "Has Department Function"),
};
