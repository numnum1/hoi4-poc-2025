#pragma once

#include "CoreMinimal.h"
#include "RolePermission.generated.h"

/**
 * What a role in an organization grants. An action's RequiredPermissions mask is checked against
 * the acting position's role, never against a role name directly — new roles are then free to
 * combine existing permissions instead of needing every action updated.
 *
 * Both Bitflags and UseEnumValuesAsMaskValuesInEditor are required together: without the latter
 * the editor treats the enum values as bit-shift indices instead of the masks they already are,
 * and silently sets the wrong bit with no compile error.
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ERolePermission : uint8
{
	RP_None              = 0      UMETA(Hidden),
	RP_PublishAlignment  = 1 << 0 UMETA(DisplayName = "Publish Alignment"),
	RP_SettleNation      = 1 << 1 UMETA(DisplayName = "Settle Nation"),
	RP_Dissolve          = 1 << 2 UMETA(DisplayName = "Dissolve"),
};
