#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapModes/MapModeType.h"
#include "MapModes/RtsMapModeContext.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "Styling/SlateBrush.h"
#include "RtsMapModes.generated.h"

/**
 * The map mode catalogue, hardcoded as small switches over EMapMode rather than a registry or a
 * UObject subclass per mode. Adding a mode means adding one EMapMode entry and one case to each
 * switch here — nothing else.
 *
 * Lives in RtsUI and not in WorldMap because a mode reads several domain facades at once, which
 * only RtsUI is allowed to do. WorldMap stays a renderer: it is handed colors and knows nothing
 * about why a province is that color.
 *
 * A UBlueprintFunctionLibrary rather than a namespace, the same shape URtsActions uses.
 */
UCLASS()
class RTSUI_API URtsMapModes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Every mode, in the order the picker should show them. */
	UFUNCTION(BlueprintPure, Category="Rts Map Modes")
	static TArray<EMapMode> GetMapModes();

	static FText GetDisplayName(EMapMode InMapMode);
	static FSlateBrush GetIcon(EMapMode InMapMode);

	/**
	 * The color of every living province under one mode — always the whole world, never a partial
	 * change set. The map actor writes what it is given without clearing, so a mode that skipped
	 * provinces would leave the previous mode's colors showing wherever the two disagree.
	 *
	 * Empty means the mode could not be built at all (a facade it needs is missing), which the
	 * caller can safely push: painting nothing leaves the map as it was.
	 */
	static TMap<FProvinceId, FProvinceVisuals> BuildVisuals(EMapMode InMapMode, const FRtsMapModeContext& InContext);

	/**
	 * A stable, readable color for an organization. FOrganizationInfo carries no map color, and
	 * adding one would mean touching every authored scenario asset, so it is derived from the id
	 * instead — golden-ratio hue steps keep neighbouring ids far apart in hue.
	 */
	static FColor GetOrganizationColor(FOrganizationId InOrganizationId);
};
