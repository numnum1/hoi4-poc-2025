#pragma once

#include "CoreMinimal.h"
#include "OrganizationFocusState.generated.h"

UENUM(BlueprintType)
enum class EOrganizationFocusState : uint8
{
	OFS_Completed   UMETA(DisplayName = "Completed"),
	OFS_InProgress  UMETA(DisplayName = "In Progress"),
	OFS_Excluded    UMETA(DisplayName = "Excluded"),
	OFS_Locked      UMETA(DisplayName = "Locked"),
	OFS_Unavailable UMETA(DisplayName = "Unavailable"),
	OFS_Available   UMETA(DisplayName = "Available"),
};
