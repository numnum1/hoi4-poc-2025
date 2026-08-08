#pragma once

#include "CoreMinimal.h"
#include "RolePermission.h"
#include "RoleInfo.generated.h"

/**
 * One authored role plus what it grants. Produced by ScenarioLoader::ExtractRoles from the
 * URoleAsset entries a scenario lists, held by URoles as the runtime library.
 *
 * FRoleId names an entry here, never a per-character state — the position it is attached to
 * (FPosition::RoleId) is the per-character part.
 */
USTRUCT(BlueprintType)
struct ORGANIZATIONS_API FRoleInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText RoleName = FText::FromString("Unnamed Role");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum="/Script/RtsCore.ERolePermission"))
	int32 Permissions = 0;

	/** Whether founding an organization should hand the founder this role. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLeadership = false;

	/** Whether joining an organization should hand the new member this role. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDefaultMembership = false;
};
