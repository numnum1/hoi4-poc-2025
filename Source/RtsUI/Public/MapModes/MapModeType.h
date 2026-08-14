#pragma once

#include "CoreMinimal.h"
#include "MapModeType.generated.h"

/**
 * Every map mode that exists, hardcoded — there is no per-mode UObject subclass and no data asset
 * that picks one. URtsMapModes switches over this to answer everything about one mode (its name,
 * its icon, and the province colors it produces); UMapModeEntryViewModel just carries the result of
 * that switch as UI data.
 */
UENUM(BlueprintType)
enum class EMapMode : uint8
{
	/** Every province in the color of the nation that owns it. */
	EMM_Political     UMETA(DisplayName = "Political"),

	/** Where each organization holds sway, with its headquarters province picked out on the map's
	 * national channel. */
	EMM_Organizations UMETA(DisplayName = "Organizations"),

	/** Every province in the color of the ethnicity most of its people belong to. */
	EMM_Ethnicities   UMETA(DisplayName = "Ethnicities"),
};
