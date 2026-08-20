#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "MapTypes.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "RtsWorldMap.generated.h"

class ACountryName;
class UCanvasRenderTarget2D;
class UDataTable;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;
class UTexture2D;

/**
 * The map actor: the single source of truth for the province-ID lookup, and a renderer for colors
 * someone else decided on.
 *
 * This used to derive from the BloodAnvil plugin's ABA_Map. The parts of it this game actually needs
 * — the mesh and material setup, the UV projection, and the lookup textures the map material samples
 * — were moved in here so they sit in Source/ where they are versioned and reviewable; Plugins/ is
 * not in this repository. The plugin stays installed for its material graph, its meshes and its map
 * authoring tools, and the only code still read from it is the DataTable row format those tools
 * write (see EnsureMapDataLookup).
 *
 * It derives nothing beyond the province lookup. It has no opinion on who owns a province, which map
 * mode is showing, or what a click means — all of that is decided in RtsUI and pushed down through
 * UWorldMapWidget.
 */
UCLASS()
class WORLDMAP_API ARtsWorldMap : public AActor
{
	GENERATED_BODY()

public:
	ARtsWorldMap();

	/** Every click the map camera reports, resolved: an invalid id means the click landed on no
	 * province. This is the only place a click is announced — UWorldMapWidget passes it up to the
	 * HUD, and nothing raises it a second time. */
	UPROPERTY(BlueprintAssignable, Category="Province Lookup")
	FWorldMapProvinceClicked OnProvinceClicked;

	/** Takes a click on the world from ARtsMapPawn and broadcasts what it hit. The pawn traces but
	 * resolves nothing, so that MapCamera can depend on this module without this module having to
	 * depend back on it. A miss is announced as an invalid province rather than dropped, because
	 * clicking off the map is how a selection is cleared. */
	UFUNCTION(BlueprintCallable, Category="Province Lookup")
	void NotifyWorldClicked(const FHitResult& InHit);

	/** The province at a map UV, or an invalid id for open water and anything else no province
	 * claims. Reads the province-ID texture's pixel and resolves it through the province table —
	 * the reverse of the translation SetProvinceVisuals does. */
	UFUNCTION(BlueprintCallable, Category="Province Lookup")
	FProvinceId ResolveProvinceAt(FVector2D InUV);

	/** The province a province-ID color belongs to, or an invalid id for water and for colors the
	 * table does not list. Blue and alpha are ignored — only red and green carry the id. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Province Lookup")
	FProvinceId FindProvinceByColor(const FColor& InProvinceColor) const;

	/** The map table's Unit Position column for a province, straight off ProvinceMapData rather than
	 * anything derived from the province-ID texture. False if the province or its row is unknown. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Province Lookup")
	bool FindUnitPositonOfProvince(const FProvinceId& ProvinceId, FVector2D& OutUnitPosition) const;

	/** The nation owning the province a province-ID texture pixel belongs to, or an invalid id for
	 * water and for provinces no nation in NationNames claims. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Province Lookup")
	FNationId FindOwnerNationIdForProvinceColor(const FColor& InProvinceColor) const;

	/** Paints the given provinces and leaves every other province untouched. A province the table
	 * does not know is skipped rather than painted black. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	/** What a nation is called and which provinces it owns, pushed down from whatever resolved
	 * UNations — WorldMap has no dependency on the Nations module and never asks for this itself. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData);

	/** Groups InProvinces into contiguous zones, so a nation holding two separate landmasses is
	 * labelled once per landmass. OutZones is grown, not cleared: zones already in it are kept and
	 * merged into, which is how provinces are folded in batch by batch. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	bool GenerateMapNameZones(const TArray<FProvinceId>& InProvinces, TArray<FMapNameZone>& OutZones);

	/** Spawns one ACountryName per entry and hands it its label points, its text and its owner tags.
	 * Where the points along a country's shape actually come from is not decided yet. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void GenerateCountryNames();
	void GenerateCountryName(const FNationId& Id, const FWorldMapNationData& Data);
	void GenerateCountryNameForZone(const FNationId& Id, const FWorldMapNationData& Data, const FMapNameZone& Zone, const TArray<FNationId>& TraceOverTags);
protected:
	UFUNCTION(BlueprintImplementableEvent)
	ACountryName* SpawnMapName();
public:
	/** Where a world-space hit lands in map UV space. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FVector2D VectorToUV(FVector InImpactPoint) const;

	/** The inverse of VectorToUV: the point on the globe's surface (relative to the map actor) a map
	 * UV corresponds to. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FVector UVToLocation(FVector2D InUV) const;

	/** The map actor of InWorldContextObject's world. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map", meta=(WorldContext="WorldContextObject"))
	static ARtsWorldMap* GetRtsWorldMap(const UObject* WorldContextObject);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MapMesh;

	/** Sampled per pixel by the map material to tell provinces apart. This one is for the GPU only —
	 * it may be compressed, streamed and mipped however the material needs, and nothing reads its
	 * pixels on the CPU. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdTexture;

	/** The same province ids again, as a texture ResolveProvinceAt can read pixels out of.
	 *
	 * Separate from ProvinceIdTexture because the two have opposite requirements: the material wants
	 * whatever format renders best, while a CPU read needs the mip 0 bulk data to still be there and
	 * to be uncompressed BGRA8 — so this one must be VectorDisplacementmap, sRGB off, no mips, and
	 * must not be streamed. Sampling goes through UVs, so it does not have to match the GPU texture's
	 * resolution, only its layout. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdLookupTexture;

	/** Rows of FProvinceMapDataGame as the plugin's map editor writes them, keyed by the same names
	 * FProvinceDisplayInfo::MapId carries. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UDataTable> ProvinceMapData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TObjectPtr<UStaticMesh> GlobeMesh;

	/** Use r = sqrt((x*y)/4*pi) to give the globe the same surface area as the plane it replaced. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings", meta=(Units="Centimeters"))
	float MapRadius = 19947.0f;

	/** One per material slot on the mesh; anything beyond that is kept for transition or distance
	 * variants. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TArray<TObjectPtr<UMaterialInterface>> MapMaterials;

	UPROPERTY(BlueprintReadOnly, Category="Shading")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MapDynamicMaterials;

	/** Set via SetNationData. The world exists once and its nations barely change, so this is a
	 * plain pushed-down map, not something built for repeated cheap updates. */
	UPROPERTY(BlueprintReadOnly, Category="World Map")
	TMap<FNationId, FWorldMapNationData> NationNames;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	/** Sizes the mesh to MapRadius and points the dynamic materials at it. Runs on every
	 * construction, not just once, so radius/mesh edits in the editor show up immediately. */
	void ConfigureMesh();

	/** Creates the lookup textures and binds them, along with the province-ID texture, to the map
	 * material. The textures are created once; the binding is redone on every call, because
	 * RebuildDynamicMaterials hands out fresh instances that carry no parameters. */
	bool EnsureLuts();

	/** Reads the province table once. Nothing authored there changes at runtime, so this never has
	 * to be rebuilt. Const (and its caches mutable) so const lookups like
	 * FindOwnerNationIdForProvinceColor can use the same pixel resolution as everything else, instead
	 * of duplicating it. */
	void EnsureMapDataLookup() const;

	/** Resolves the table against the loaded scenario, which is the only reason this is separate:
	 * the table can be read the moment the map exists, but province ids only exist once the
	 * scenario has been loaded. Stays empty and is retried until then. */
	void EnsureProvinceLookup();

	/** Translates the map table's adjacency lists into province ids once. Separate from
	 * EnsureProvinceLookup for the same reason that one is separate: it needs the loaded scenario. */
	void EnsureAdjacencyLookup();

	/** Reads the map table's Unit Position column once, keyed by province id. Kept const (and its
	 * cache mutable) since FindUnitPositonOfProvince is a plain query, not a mutation. */
	void EnsureUnitPositionLookup() const;

	/** Builds the reverse of NationNames' OwnedProvinces once. No scenario dependency, unlike the
	 * other Ensure* lookups: NationNames is already pushed-in province ids, nothing to resolve. */
	void EnsureNationOwnershipLookup() const;

	/** The province-ID pixel at a map UV, straight off ProvinceIdLookupTexture's mip 0. False when
	 * that texture is set up in a way the CPU cannot read. */
	bool ReadLookupPixel(FVector2D InUV, FColor& OutColor) const;

	/** Whether the lookup texture is uncompressed, built and CPU-side. Checked before every read
	 * because UBA_DataFunctions::FindColor cannot report any of it: handed a compressed texture it
	 * reads the block bytes as a color and returns a province that was never clicked. */
	bool IsLookupTextureReadable() const;

	void LogLookupTextureUnreadable(const TCHAR* InReason) const;

	void RebuildDynamicMaterials();

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> PrimaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> SecondaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> NationalLut;

	/** Nothing writes this one. It is still created and bound because the map material samples it,
	 * and leaving the parameter unset would fall back to whatever default the material carries
	 * instead of the black it has always been handed. */
	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> VfxLut;

	//~ The lookup caches below are Transient on purpose: every one of them is rebuilt from
	//~ ProvinceMapData, which is the only place this data is authored. Saving a copy into the map
	//~ actor would let the two disagree after an edit to the table. They are reflected so they can be
	//~ read in the details panel while the game runs, not because anything binds them.

	/** MapId -> the province-ID pixel the lookup textures key their rows on. Mutable: populated
	 * lazily by the const EnsureMapDataLookup. */
	UPROPERTY(VisibleAnywhere, Transient, Category="Province Lookup")
	mutable TMap<FName, FMapPixelId> MapIdToPixelId;

	/** The reverse of MapIdToPixelId, and the resolve direction: the color sampled off the
	 * province-ID texture back to the table row that declared it. Built from the same table pass, so
	 * it needs no loaded scenario — unlike ProvinceIdToPixelId, which resolves ids as well. */
	UPROPERTY(VisibleAnywhere, Transient, Category="Province Lookup")
	mutable TMap<FMapPixelId, FName> PixelIdToMapId;

	/** What painting actually goes through, so a repaint does not copy a whole FProvinceDisplayInfo
	 * — FText, FSlateBrush and two object pointers — once per province just to read its MapId. */
	UPROPERTY(VisibleAnywhere, Transient, Category="Province Lookup")
	TMap<FProvinceId, FMapPixelId> ProvinceIdToPixelId;

	/** Who borders whom, as the map table's FName adjacency lists resolve to province ids. Neighbours
	 * outside the loaded scenario are dropped rather than kept as invalid ids.
	 *
	 * The one lookup that is not reflected: UHT has no property type for a container inside a
	 * container, so showing this in the details panel would mean wrapping the inner array in a
	 * USTRUCT and touching every use of it. */
	TMap<FProvinceId, TArray<FProvinceId>> ProvinceAdjacencies;

	/** Provinces with at least one neighbour the loaded scenario does not know, which ProvinceAdjacencies
	 * cannot express — those neighbours have no province id. Counts as bordering. */
	UPROPERTY(VisibleAnywhere, Transient, Category="Province Lookup")
	TSet<FProvinceId> ProvincesAdjacentToUnknown;

	/** Province id -> the map table's Unit Position column. See EnsureUnitPositionLookup. */
	UPROPERTY(VisibleAnywhere, Transient, Category="Province Lookup")
	mutable TMap<FProvinceId, FVector2D> ProvinceUnitPositions;

	/** Province id -> the nation owning it, per NationNames. See EnsureNationOwnershipLookup. */
	UPROPERTY(VisibleAnywhere, Transient, Category="Province Lookup")
	mutable TMap<FProvinceId, FNationId> ProvinceOwnerNationId;

	/** The tallest row any province occupies, which is what decides the lookup textures' height. */
	UPROPERTY(VisibleAnywhere, Transient, Category="Province Lookup")
	mutable int32 HighestPixelRow = 0;

	/** So a map missing its assets is reported once, not per click. */
	mutable bool bLookupFailureLogged = false;

	/** Likewise for a lookup texture that is present but unreadable — a separate flag, because that is
	 * a different fix than a missing asset and both would otherwise silence each other. */
	mutable bool bLookupTextureFailureLogged = false;

	/** Likewise for painting, which is retried on every repaint. */
	bool bPaintFailureLogged = false;
};
