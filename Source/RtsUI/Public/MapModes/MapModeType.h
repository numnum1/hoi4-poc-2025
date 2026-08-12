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
	/** The owning nation's color. Produces no overrides at all — ARtsWorldMap already derives this
	 * as its default shading, so the mode exists to switch *back* to it. */
	EMM_Political     UMETA(DisplayName = "Political"),

	/** Where each organization holds sway, with its headquarters province picked out on the map's
	 * national channel. */
	EMM_Organizations UMETA(DisplayName = "Organizations"),
};
