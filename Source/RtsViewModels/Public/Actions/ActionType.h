#pragma once

#include "CoreMinimal.h"
#include "ActionType.generated.h"

/**
 * Every action that exists, hardcoded — there is no per-action UObject subclass. URtsActions
 * switches over this to answer everything about one action (scope, display data, permissions,
 * what running it does); UActionViewModel just carries the result of that switch as UI data.
 */
UENUM(BlueprintType)
enum class EAction : uint8
{
	EA_FoundOrganization UMETA(DisplayName = "Found Organization"),
	EA_JoinOrganization  UMETA(DisplayName = "Join Organization"),
	EA_LeaveOrganization UMETA(DisplayName = "Leave Organization"),
	EA_RecruitCharacter  UMETA(DisplayName = "Recruit Character"),
};
