#pragma once

#include "CoreMinimal.h"
#include "PlayerModal.generated.h"

/**
 * Which modal the player-facing UI currently has open. Widget state in the strict sense — nothing
 * in the simulation knows a modal exists — which is why it lives on UPlayerCharacterViewModel
 * and not on any view model that is a pure derivation of the simulation.
 *
 * One enum plus a switch rather than a class per dialog or a bool per dialog: PM_None is a real
 * state, so "is anything open" never needs a second flag to stay in sync.
 */
UENUM(BlueprintType)
enum class EPlayerModal : uint8
{
	PM_None              UMETA(DisplayName = "None"),
	PM_FoundOrganization UMETA(DisplayName = "Found Organization"),
	PM_JoinOrganization  UMETA(DisplayName = "Join Organization"),
	PM_ViewProvince      UMETA(DisplayName = "View Province"),
	PM_ViewOrganization  UMETA(DisplayName = "View Organization"),
	PM_RecruitCharacter  UMETA(DisplayName = "Recruit Character")
};
