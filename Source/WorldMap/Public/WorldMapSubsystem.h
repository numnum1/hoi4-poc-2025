#pragma once

#include "CoreMinimal.h"
#include "MapTypes.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldMapSubsystem.generated.h"

class UDataTable;
class UTexture2D;

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
	static UWorldMapSubsystem* Get(const UObject* InWorldContextObject);

	void Configure(UDataTable* InProvinceMapData);

	bool HasMapData() const;

	bool FindPixelId(FProvinceId InProvinceId, FMapPixelId& OutPixelId) const;

	TArray<FMapLutEntry> BuildLutRows(const TMap<FProvinceId, FProvinceVisuals>& InVisuals) const;

	/** Turns a texel of the province-ID texture into the id the map table and the lookup textures
	 * use. Green is scaled back by the highest row in use - see FMapPixelId. */
	FMapPixelId DecodePixel(const FColor& InTexelColor) const;

	FProvinceId FindProvince(const FMapPixelId& InPixelId) const;

	bool FindUnitPosition(FProvinceId InProvinceId, FVector2D& OutUnitPosition) const;

	int32 GetHighestPixelRow() const;

	bool BuildNameZones(const TArray<FProvinceId>& InProvinces, TArray<FMapNameZone>& OutZones) const;

	/** Debug: whether the ID texture the map renders and the province table agree at all. */
	void LogIdTextureCoverage(UTexture2D* InTexture) const;

private:
	void EnsureMapDataLookup() const;

	void EnsureProvinceLookup() const;

	void EnsureAdjacencyLookup() const;

	void EnsureUnitPositionLookup() const;

	void LogResolvedPixel(const FMapPixelId& InPixelId, FProvinceId InProvinceId) const;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> ProvinceMapData;

	mutable TMap<FName, FMapPixelId> MapIdToPixelId;

	mutable TMap<FProvinceId, FMapPixelId> ProvinceIdToPixelId;

	mutable TMap<FMapPixelId, FProvinceId> PixelIdToProvinceId;

	mutable TMap<FProvinceId, TArray<FProvinceId>> ProvinceAdjacencies;

	mutable TSet<FProvinceId> ProvincesAdjacentToUnknown;

	mutable TMap<FProvinceId, FVector2D> ProvinceUnitPositions;

	mutable int32 HighestPixelRow = 0;

	mutable int32 HighestPixelColumn = 0;

	mutable bool bMissingTableLogged = false;

	mutable bool bNoMatchingProvincesLogged = false;
};
