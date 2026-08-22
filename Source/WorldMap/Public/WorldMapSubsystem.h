#pragma once

#include "CoreMinimal.h"
#include "MapTypes.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldMapSubsystem.generated.h"

class UDataTable;

/**
 * Everything the map knows that is a province rather than a pixel: the map table, both directions of
 * the province lookup, who borders whom, and where a province's unit marker sits.
 *
 * It lives beside ARtsWorldMap rather than inside it because none of it is rendering. The actor
 * paints pixels and reads pixels; anything that has to turn a pixel into an FProvinceId — or an
 * FProvinceId into a pixel — asks here. That is what keeps the actor free of the province concept
 * altogether.
 *
 * The table itself is still authored on the map actor, which hands it over through Configure: the
 * level is where a designer picks which world is loaded, and a subsystem has no details panel to
 * pick it in.
 *
 * Every lookup below is built lazily and cached. The ones that resolve province ids need the loaded
 * scenario and stay empty until it is there, so they are retried rather than built once.
 */
UCLASS()
class WORLDMAP_API UWorldMapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** The map subsystem of InWorldContextObject's world, or nullptr outside a world. */
	static UWorldMapSubsystem* Get(const UObject* InWorldContextObject);

	/** Hands over the map table. Called by ARtsWorldMap, which is where the table is authored; a
	 * different table drops every cache built from the previous one. */
	void Configure(UDataTable* InProvinceMapData);

	/** Whether the map table has been read and holds rows. True without a loaded scenario — the
	 * table alone is enough to size the lookup textures. */
	bool HasMapData() const;

	/** The pixel a province occupies in the province-ID texture, and with it the row its colors are
	 * written to. False for a province the map table does not list. */
	bool FindPixelId(FProvinceId InProvinceId, FMapPixelId& OutPixelId) const;

	/** The given provinces as rows the map actor can write straight into its lookup textures.
	 * Provinces the map table does not list are skipped rather than turned into a black row. */
	TArray<FMapLutEntry> BuildLutRows(const TMap<FProvinceId, FProvinceVisuals>& InVisuals) const;

	/** The province a province-ID pixel belongs to, or an invalid id for open water and for pixels
	 * the table does not list. */
	FProvinceId FindProvince(const FMapPixelId& InPixelId) const;

	/** The map table's Unit Position column for a province — the anchor anything sitting on a
	 * province is placed at. False if the province or its row is unknown. */
	bool FindUnitPosition(FProvinceId InProvinceId, FVector2D& OutUnitPosition) const;

	/** The tallest row any province occupies, which is what the lookup textures' height has to be.
	 * Available without a loaded scenario — it comes off the table alone. */
	int32 GetHighestPixelRow() const;

	/** Groups InProvinces into contiguous zones, so a nation holding two separate landmasses is
	 * labelled once per landmass. OutZones is grown, not cleared: zones already in it are kept and
	 * merged into, which is how provinces are folded in batch by batch. */
	bool BuildNameZones(const TArray<FProvinceId>& InProvinces, TArray<FMapNameZone>& OutZones) const;

private:
	/** Reads the province table once. Nothing authored there changes at runtime, so this never has to
	 * be rebuilt, and it needs no scenario — unlike everything below it. */
	void EnsureMapDataLookup() const;

	/** Resolves the table's row names against the loaded scenario. Separate from the table pass
	 * because the table can be read the moment the world exists, while province ids only exist once
	 * a scenario has been loaded. */
	void EnsureProvinceLookup() const;

	/** Translates the table's adjacency lists into province ids. Needs the loaded scenario for the
	 * same reason EnsureProvinceLookup does. */
	void EnsureAdjacencyLookup() const;

	/** Reads the table's Unit Position column, keyed by province id. */
	void EnsureUnitPositionLookup() const;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> ProvinceMapData;

	//~ The caches below are all rebuilt from ProvinceMapData, which is the only place this data is
	//~ authored, and they are mutable because every query above is const.

	/** MapId -> the province-ID pixel the lookup textures key their rows on. */
	mutable TMap<FName, FMapPixelId> MapIdToPixelId;

	/** What painting goes through, so a repaint does not resolve a name per province. */
	mutable TMap<FProvinceId, FMapPixelId> ProvinceIdToPixelId;

	/** The reverse of ProvinceIdToPixelId, which is what a click needs. */
	mutable TMap<FMapPixelId, FProvinceId> PixelIdToProvinceId;

	/** Who borders whom. Neighbours outside the loaded scenario are dropped rather than kept as
	 * invalid ids. */
	mutable TMap<FProvinceId, TArray<FProvinceId>> ProvinceAdjacencies;

	/** Provinces with at least one neighbour the loaded scenario does not know, which
	 * ProvinceAdjacencies cannot express — those neighbours have no province id. Counts as
	 * bordering. */
	mutable TSet<FProvinceId> ProvincesAdjacentToUnknown;

	/** Province id -> the map table's Unit Position column. */
	mutable TMap<FProvinceId, FVector2D> ProvinceUnitPositions;

	/** The tallest row any province occupies. See GetHighestPixelRow. */
	mutable int32 HighestPixelRow = 0;

	/** So a missing table is reported once, not per click. */
	mutable bool bMissingTableLogged = false;

	/** Likewise for a table whose rows match no province of the loaded scenario — a different fix
	 * than a missing table, and both would otherwise silence each other. */
	mutable bool bNoMatchingProvincesLogged = false;
};
