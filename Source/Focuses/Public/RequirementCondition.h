#pragma once

#include "CoreMinimal.h"
#include "BuildingFunction.h"
#include "RequirementConditionType.h"
#include "RequirementCondition.generated.h"

class URequirementAsset;

USTRUCT(BlueprintType)
struct FOCUSES_API FRequirementCondition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ERequirementConditionType Type = ERequirementConditionType::RCT_MinOrganizationFunds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bNegate = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="Type == ERequirementConditionType::RCT_MinOrganizationFunds", EditConditionHides))
	int32 Amount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="Type == ERequirementConditionType::RCT_HasDepartmentFunction", EditConditionHides))
	EBuildingFunction Function = EBuildingFunction::BF_Bureaucracy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="Type == ERequirementConditionType::RCT_Nested", EditConditionHides))
	TObjectPtr<URequirementAsset> Nested;
};
