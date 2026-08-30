#pragma once

#include "CoreMinimal.h"
#include "BuildingFunction.generated.h"

/**
 * What a department does, and what a building offers space for — the same vocabulary from both
 * sides, because a department can only be seated where its function is on offer.
 *
 * This is also the only thing that says what kind of organization something is: there is no
 * organization type and no department type asset. An organization is a church because its
 * departments pray, which is a fact about what it does rather than a label somebody picked.
 *
 * Lives in RtsCore rather than in Buildings or Organizations because both need it and the domain
 * modules do not know each other — the same reason ERolePermission is here. What a function
 * actually produces will be a switch over these values, never a data asset selecting behaviour or
 * a class per function; see URtsActions for the shape.
 */
UENUM(BlueprintType)
enum class EBuildingFunction : uint8
{
	BF_Bureaucracy	UMETA(DisplayName = "Bureaucracy"),
	BF_Prayer		UMETA(DisplayName = "Prayer"),
	BF_Production	UMETA(DisplayName = "Production"),
	BF_Retail		UMETA(DisplayName = "Retail"),
};
