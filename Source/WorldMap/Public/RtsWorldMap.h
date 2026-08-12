#pragma once

#include "CoreMinimal.h"
#include "BA_Map.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "RtsWorldMap.generated.h"

/**
 * The game's ABA_Map subclass, and deliberately nothing more than a renderer.
 *
 * It answers exactly one question — "paint these provinces these colors" — by translating province
 * ids into the province-ID pixel colors the plugin's lookup tables are keyed on and handing the
 * result to WriteToLUTs. It derives nothing: it has no opinion on who owns a province, which map
 * mode is showing, or what a click means. All of that is decided in RtsUI and pushed down through
 * UWorldMapWidget, which is why this class holds no state of its own.
 *
 * The plugin's Blueprint side keeps owning the province-ID texture, the province DataTable and the
 * lookup-table setup (InitialiseBaseLUTs) — this is built around that, not over it.
 */
UCLASS()
class WORLDMAP_API ARtsWorldMap : public ABA_Map
{
	GENERATED_BODY()

public:
	/** Paints the given provinces and leaves every other province untouched. Provinces whose id the
	 * simulation does not know a pixel color for are skipped rather than painted black. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	/** The map actor of InWorldContextObject's world, so a caller outside WorldMap can reach it
	 * without depending on the BloodAnvil plugin itself. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map", meta=(WorldContext="WorldContextObject"))
	static ARtsWorldMap* GetRtsWorldMap(const UObject* WorldContextObject);

protected:
	/** ABA_Map::OnConstruction appends to MapDynamicMaterials without clearing it first, so every
	 * reconstruction in the editor leaves another generation of dead dynamic materials behind.
	 * Emptying it here is the fix from outside the plugin. */
	virtual void OnConstruction(const FTransform& Transform) override;
};
