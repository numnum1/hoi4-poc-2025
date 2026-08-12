#pragma once

#include "CoreMinimal.h"
#include "BA_Map.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "RtsWorldMap.generated.h"

class UDataTable;
class UTexture2D;

/**
 * The game's ABA_Map subclass: the single source of truth for the province-ID lookup, and a
 * renderer for colors someone else decided on.
 *
 * The province-ID texture and the province DataTable are not declared here — they are already
 * authored on the plugin's map Blueprint, and declaring them again would mean two places to set the
 * same asset. GetProvinceIdTexture/GetProvinceMapData reach into that one place instead, so
 * everything that needs either (this class, and ARtsMapPawn through ResolveProvinceAt) reads the
 * same assets.
 *
 * It derives nothing beyond that lookup. It has no opinion on who owns a province, which map mode
 * is showing, or what a click means — all of that is decided in RtsUI and pushed down through
 * UWorldMapWidget.
 */
UCLASS()
class WORLDMAP_API ARtsWorldMap : public ABA_Map
{
	GENERATED_BODY()

public:
	/** The province-ID texture the map material samples. Implemented by the map Blueprint, which is
	 * where the asset is authored — one node returning its existing variable. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category="Province Lookup")
	UTexture2D* GetProvinceIdTexture();

	/** Rows of FProvinceMapDataGame (BA_DataFunctions.h), keyed by the same names
	 * FProvinceDisplayInfo::MapId carries. Implemented by the map Blueprint for the same reason as
	 * GetProvinceIdTexture. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category="Province Lookup")
	UDataTable* GetProvinceMapData();

	/** The province at a map UV, or an invalid id for open water and anything else no province
	 * claims. Reads the province-ID texture's pixel and resolves it through the province table —
	 * the reverse of the translation SetProvinceVisuals does. */
	UFUNCTION(BlueprintCallable, Category="Province Lookup")
	FProvinceId ResolveProvinceAt(FVector2D InUV);

	/** Paints the given provinces and leaves every other province untouched. A province the table
	 * does not know is skipped rather than painted black. */
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

private:
	/** Reads the province table once into both directions of the lookup. Nothing authored there
	 * changes at runtime, so this never has to be rebuilt. */
	void EnsureProvinceLookup();

	/** MapId -> the province-ID pixel color WriteToLUTs keys its rows on. */
	TMap<FName, FColorRG> MapIdToPixelColor;

	/** The reverse, keyed on the red and green channels packed together: those two carry the id,
	 * and a pixel sampled off the texture brings along a blue and alpha the table never declares. */
	TMap<int32, FName> PixelColorToMapId;

	/** So a Blueprint that never implemented the two lookup events is reported once, not per click. */
	bool bLookupFailureLogged = false;
};
