#pragma once

#include "CoreMinimal.h"
#include "EventEffectType.generated.h"

UENUM(BlueprintType)
enum class EEventEffectType : uint8
{
	EET_ChangeOrganizationFunds UMETA(DisplayName = "Change Organization Funds"),
};
