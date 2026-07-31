// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuildingOwnerType.generated.h"

/**
 * Which kind of owner a building has. The value itself is never stored on the entity — it is
 * read back from which owner fragment is present (FBuildingCharacterOwnerFragment xor
 * FBuildingOrganizationOwnerFragment), so this enum exists only at the Blueprint/facade boundary.
 */
UENUM(BlueprintType)
enum class EBuildingOwnerType : uint8
{
	/** No living owner fragment, e.g. the building id itself is not alive. */
	BOT_None         UMETA(DisplayName = "None"),
	BOT_Character    UMETA(DisplayName = "Character"),
	BOT_Organization UMETA(DisplayName = "Organization")
};
