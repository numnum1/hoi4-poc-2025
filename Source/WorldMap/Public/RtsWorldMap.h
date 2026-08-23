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

	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapPixelClicked OnPixelClicked;

	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapPixelHovered OnPixelHovered;

	UFUNCTION(BlueprintCallable, Category="World Map")
	void NotifyWorldClicked(const FHitResult& InHit);

	/** Called every frame by ARtsMapPawn with whatever is under the cursor. Reads nothing and
	 * broadcasts nothing while the highlight is off, and otherwise only when the pixel changes -
	 * the caller is a tick, so everything below has to be cheap on the frames where nothing moved. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void NotifyWorldHovered(const FHitResult& InHit);

	UFUNCTION(BlueprintCallable, Category="World Map")
	void PaintProvinces(const TArray<FMapLutEntry>& InRows);

	/** Writes the VFX layer alone, from FMapLutEntry::Vfx. Separate from PaintProvinces because the
	 * highlight changes on its own schedule and must not drag the colors of a whole map mode through
	 * the render thread with it. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void PaintProvinceVfx(const TArray<FMapLutEntry>& InRows);

	/** Marks one pixel as the highlighted one: its VFX row is set and its colors are brightened on
	 * top of whatever was last painted there. Held here rather than by the caller because the actor
	 * is the only thing that knows what each row currently holds - PaintProvinces would otherwise
	 * wipe the highlight, and a caller restoring it from its own copy of the colors would fight
	 * anyone else painting the map. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetHighlightedPixel(FMapPixelId InPixelId);

	UFUNCTION(BlueprintCallable, Category="World Map")
	void ClearHighlightedPixel();

	/** Pushed down from URtsWorldMapViewModel, which owns the switch - this is the copy the pawn's
	 * trace and NotifyWorldHovered read, not a second opinion about it. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetHoverHighlightEnabled(bool bInEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	bool IsHoverHighlightEnabled() const { return bHoverHighlightEnabled; }

	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceOwners(const TMap<FMapPixelId, FNationId>& InOwners);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FNationId FindOwnerNationIdForProvinceColor(const FColor& InProvinceColor) const;

	UFUNCTION(BlueprintCallable, Category="World Map")
	void SpawnCountryLabels(const TArray<FWorldMapLabel>& InLabels);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	ACountryName* SpawnMapName();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FVector2D VectorToUV(FVector InImpactPoint) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map")
	FVector UVToLocation(FVector2D InUV) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map", meta=(WorldContext="WorldContextObject"))
	static ARtsWorldMap* GetRtsWorldMap(const UObject* WorldContextObject);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MapMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Province Lookup")
	TObjectPtr<UDataTable> ProvinceMapData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TObjectPtr<UStaticMesh> GlobeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings", meta=(Units="Centimeters"))
	float MapRadius = 19947.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	TArray<TObjectPtr<UMaterialInterface>> MapMaterials;

	/** What is written into the hovered province's row of the VFX lookup texture; black is what every
	 * other row holds and therefore means "not highlighted". Editable at runtime because which
	 * channel of that texture the map material's selection outline reads is not visible from here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings")
	FColor HoverVfxColor = FColor::White;

	/** How far the hovered province's own colors are lerped toward white, on top of the material's
	 * outline. 0 leaves the fill alone and reduces the highlight to the outline again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Settings", meta=(ClampMin="0.0", ClampMax="1.0"))
	float HoverHighlightStrength = 0.35f;

	UPROPERTY(BlueprintReadOnly, Category="Shading")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MapDynamicMaterials;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	void ConfigureMesh();

	bool EnsureLuts();

	void ConfigureSubsystem() const;

	bool ReadLookupPixel(FVector2D InUV, FColor& OutColor) const;

	/** The one way a hit on the globe becomes a pixel, shared by the click and the hover so the two
	 * can never disagree about which province the cursor is on. */
	bool ResolvePixel(const FHitResult& InHit, FMapPixelId& OutPixelId) const;

	/** Writes the three color layers without recording the rows, so the brightened highlight never
	 * becomes the province's remembered color. */
	void WriteColorRows(const TArray<FMapLutEntry>& InRows);

	void RepaintHighlight(bool bInHadPrevious, FMapPixelId InPreviousPixelId);

	void RebuildDynamicMaterials();

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> PrimaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> SecondaryLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> NationalLut;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> VfxLut;

	UPROPERTY(VisibleAnywhere, Transient, Category="World Map")
	TMap<FMapPixelId, FNationId> PixelOwnerNationId;

	UPROPERTY(EditAnywhere, Category="Map Settings")
	bool bHoverHighlightEnabled = true;

	/** What was last painted into each row, whoever painted it. The highlight is applied on top of
	 * this, so it survives a map mode change and does not erase an overlay someone else put there. */
	TMap<FMapPixelId, FMapLutEntry> PaintedRows;

	FMapPixelId HighlightedPixelId;

	bool bHasHighlight = false;

	FMapPixelId LastHoveredPixelId;

	/** The hit the last pixel was read from. A still cursor over a still globe hits the same point
	 * every frame, and skipping on that spares a texture lock per frame. */
	FVector LastHoverImpactPoint = FVector::ZeroVector;

	bool bHasHovered = false;

	bool bPaintFailureLogged = false;
};
