#pragma once

#include "CoreMinimal.h"
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

	/** The province at a map UV, or an invalid id for open water and anything else no province
	 * claims. Reads the province-ID texture's pixel and resolves it through the province table —
	 * the reverse of the translation SetProvinceVisuals does. */
	UFUNCTION(BlueprintCallable, Category="Province Lookup")
	FProvinceId ResolveProvinceAt(FVector2D InUV);

	/** The map table's Unit Position column for a province, straight off ProvinceMapData rather than
	 * anything derived from the province-ID texture. False if the province or its row is unknown. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Province Lookup")
	bool FindUnitPositonOfProvince(const FProvinceId& ProvinceId, FVector2D& OutUnitPosition) const;

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

	/** Sampled per pixel by the map material to tell provinces apart, and read on the CPU to resolve
	 * clicks. Must stay uncompressed and CPU-readable (VectorDisplacementmap, sRGB off, no mips) —
	 * ResolveProvinceAt reads its mip 0 directly. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdTexture;

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
	 * to be rebuilt. */
	void EnsureMapDataLookup();

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

	/** MapId -> the province-ID pixel the lookup textures key their rows on. */
	TMap<FName, FMapPixelId> MapIdToPixelId;

	/** The reverse, keyed on the red and green channels packed together: those two carry the id,
	 * and a pixel sampled off the texture brings along a blue and alpha the table never declares. */
	TMap<int32, FName> PixelIdToMapId;

	/** What painting actually goes through, so a repaint does not copy a whole FProvinceDisplayInfo
	 * — FText, FSlateBrush and two object pointers — once per province just to read its MapId. */
	TMap<FProvinceId, FMapPixelId> ProvinceIdToPixelId;

	/** Who borders whom, as the map table's FName adjacency lists resolve to province ids. Neighbours
	 * outside the loaded scenario are dropped rather than kept as invalid ids. */
	TMap<FProvinceId, TArray<FProvinceId>> ProvinceAdjacencies;

	/** Province id -> the map table's Unit Position column. See EnsureUnitPositionLookup. */
	mutable TMap<FProvinceId, FVector2D> ProvinceUnitPositions;

	/** The tallest row any province occupies, which is what decides the lookup textures' height. */
	int32 HighestPixelRow = 0;

	/** So a map missing its assets is reported once, not per click. */
	bool bLookupFailureLogged = false;

	/** Likewise for painting, which is retried on every repaint. */
	bool bPaintFailureLogged = false;
};
