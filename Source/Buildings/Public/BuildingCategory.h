#pragma once

#include "CoreMinimal.h"
#include "BuildingCategory.generated.h"

/**
 * Which section of the build menu a building kind appears under. Authored on UBuildingType
 * rather than derived from its FBuildingFunctionSlots: what a building lets a department *do* and
 * where a player looks for it in the menu are different questions, and a building type with no
 * authored functions at all still has to appear somewhere.
 */
UENUM(BlueprintType)
enum class EBuildingCategory : uint8
{
	BC_Administration	UMETA(DisplayName = "Administration"),
	BC_Religion			UMETA(DisplayName = "Religion"),
	BC_Industry			UMETA(DisplayName = "Industry"),
	BC_Commerce			UMETA(DisplayName = "Commerce"),
};
