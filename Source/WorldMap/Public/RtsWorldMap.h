#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "MapTypes.h"
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
 * The globe, as a thing that draws: a mesh, the map material, the lookup textures its colors come
 * out of, and the province-ID texture a click is read from. Nothing else.
 *
 * It is a view, in the same sense a widget is. It does not know what a province is — colors arrive
 * already keyed by pixel (FMapLutEntry) and a click leaves as the pixel that was read. Turning
 * either into an FProvinceId is UWorldMapSubsystem's job, and deciding what a color or a click
 * means happens far above, in RtsUI. That is what URtsWorldMapViewModel and UWorldMapWidget are
 * for: they own what is shown, this owns how it is drawn.
 *
 * This used to derive from the BloodAnvil plugin's ABA_Map. The parts of it this game actually needs
 * were moved in here so they sit in Source/ where they are versioned and reviewable; Plugins/ is not
 * in this repository. The plugin stays installed for its material graph, its meshes and its map
 * authoring tools.
 */
UCLASS()
class WORLDMAP_API ARtsWorldMap : public AActor
{
	GENERATED_BODY()

public:
	ARtsWorldMap();

	/** Every click the map camera reports, as the pixel it landed on — not as a province. A zeroed
	 * pixel is open water, which is how a selection is cleared. UWorldMapWidget resolves it and
	 * passes it up; nothing else should bind this. */
	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapPixelClicked OnPixelClicked;

	/** Takes a click on the world from ARtsMapPawn and reports the pixel under it. The pawn traces
	 * but resolves nothing, so that MapCamera can depend on this module without this module having
	 * to depend back on it. A miss is announced as a zeroed pixel rather than dropped, because
	 * clicking off the map is how a selection is cleared. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void NotifyWorldClicked(const FHitResult& InHit);

	/** Writes the given rows into the lookup textures and leaves every other row untouched. The rows
	 * arrive already keyed by pixel — whoever built them did the province lookup. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void PaintProvinces(const TArray<FMapLutEntry>& InRows);

	/** Who owns which pixel, for the country labels to trace along. Pushed down already translated,
	 * so that answering FindOwnerNationIdForProvinceColor needs no province lookup here. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceOwners(const TMap<FMapPixelId, FNationId>& InOwners);

	/** The nation owning the province a province-ID texture pixel belongs to, per the last
	 * SetProvinceOwners. Invalid for water and for pixels no nation claims.
	 *
	 * Called per traced point by the country-label Blueprint, which is why it stays a plain map
	 * lookup here rather than a trip through the subsystem. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FNationId FindOwnerNationIdForProvinceColor(const FColor& InProvinceColor) const;

	/** Spawns one ACountryName per label and hands it its points, its text and its owner tags. The
	 * points arrive as UVs and are projected here, because the globe's radius is this actor's
	 * business and nobody else's. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SpawnCountryLabels(const TArray<FWorldMapLabel>& InLabels);

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

	/** The same province ids again, as a texture a click can be read out of.
	 *
	 * Separate from ProvinceIdTexture because the two have opposite requirements: the material wants
	 * whatever format renders best, while a CPU read needs the mip 0 bulk data to still be there and
	 * to be uncompressed BGRA8 — so this one must be VectorDisplacementmap, sRGB off, no mips, and
	 * must not be streamed. Sampling goes through UVs, so it does not have to match the GPU texture's
	 * resolution, only its layout. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdLookupTexture;

	/** Rows of FProvinceMapDataGame as the plugin's map editor writes them, keyed by the same names
	 * FProvinceDisplayInfo::MapId carries. Authored here because the level is where a designer picks
	 * which world is loaded, but read by UWorldMapSubsystem, which is handed it on construction. */
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

	/** Hands ProvinceMapData to UWorldMapSubsystem. Called on every construction rather than once,
	 * so swapping the table in the editor takes effect without a restart. */
	void ConfigureSubsystem() const;

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

	/** Pixel -> the nation owning it, exactly as SetProvinceOwners was handed it. Transient because
	 * it is pushed down whenever the nations change and is never authored here. */
	UPROPERTY(VisibleAnywhere, Transient, Category="World Map")
	TMap<FMapPixelId, FNationId> PixelOwnerNationId;

	/** So a lookup texture that cannot be read is reported once, not per click. */
	mutable bool bLookupTextureFailureLogged = false;

	/** Likewise for painting, which is retried on every repaint. */
	bool bPaintFailureLogged = false;
};
