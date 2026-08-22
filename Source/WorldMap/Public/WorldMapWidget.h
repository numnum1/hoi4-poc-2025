#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapTypes.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "WorldMapWidget.generated.h"

class ARtsWorldMap;
class URtsWorldMapViewModel;

/**
 * The world map's interface to the rest of the UI, and nothing else: colors and nations arrive
 * through the setters, clicks leave through OnProvinceClicked. It exists so the map can be dropped
 * anywhere in the HUD's widget tree without whoever places it having to know about the map actor,
 * the subsystem or the camera pawn.
 *
 * It owns URtsWorldMapViewModel the same way UPlayedCharacterView owns UPlayerCharacterViewModel:
 * created in NativeOnInitialized, handed everything that is pushed in, and exposed through a getter
 * so the Blueprint binds its own widgets to it with creation type "Property Path".
 *
 * It decides nothing. It does not know what a province click means, and it has no opinion on which
 * colors are correct — both are settled in RtsUI, which owns the map mode and pushes the result
 * down here. A C++ dependency in the other direction would make WorldMap depend on RtsUI while
 * RtsUI already depends on WorldMap, a cycle.
 */
UCLASS(Abstract)
class WORLDMAP_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Fires for every click on the map; an invalid id means the player clicked off any province.
	 * The one way a click leaves this module. */
	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapProvinceClicked OnProvinceClicked;

	/** Paints the given provinces. Provinces left out keep whatever color they had — this is a
	 * change set, not a whole world, so the caller decides what "reset" means. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	/** Hands the map who owns what and what they are called, which is what its country labels are
	 * built from. Like the colors, this is decided in RtsUI and pushed down. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData);

	UFUNCTION(BlueprintPure, Category="World Map")
	URtsWorldMapViewModel* GetWorldMapViewModel() const { return WorldMapViewModel; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	/** Turns the pixel the map read into the province it belongs to. The actor reports pixels
	 * because it has no province lookup of its own — this is where that is undone. */
	UFUNCTION()
	void HandlePixelClicked(FMapPixelId InPixelId);

	/** The map actor, found once and kept. Retried on every call rather than resolved in
	 * NativeConstruct alone, because a widget built before the level's actors are up would
	 * otherwise stay pointed at nothing for the rest of its life. */
	ARtsWorldMap* ResolveWorldMap();

	UPROPERTY(Transient)
	TObjectPtr<URtsWorldMapViewModel> WorldMapViewModel;

	UPROPERTY(Transient)
	TObjectPtr<ARtsWorldMap> WorldMap;

	/** So a level without a map actor is reported once, not on every repaint. */
	bool bMissingMapLogged = false;
};
