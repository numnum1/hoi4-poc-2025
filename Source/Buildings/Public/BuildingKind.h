#pragma once

#include "CoreMinimal.h"
#include "BuildingKind.generated.h"

/**
 * Every kind of building that exists, hardcoded — there is no data asset per building type.
 * UBuildingKinds switches over this to answer everything about one kind (display data, build cost,
 * whether it is a headquarters); the entity carries the value in FBuildingKindFragment.
 *
 * This enum is also what makes an organization what it is: an organization has no type of its own,
 * it is a party because its headquarters is a party office. See URtsActions::EA_FoundOrganization.
 */
UENUM(BlueprintType)
enum class EBuildingKind : uint8
{
	/** No kind — nothing picked yet, or a building that is not there. A real state rather than a
	 * separate "has a kind" flag beside it, the same way EPlayerModal::PM_None is. No entity ever
	 * carries it: UBuildings never creates a building of no kind. */
	BK_None        UMETA(DisplayName = "None"),
	BK_PartyOffice UMETA(DisplayName = "Party Office"),
};
