#pragma once

#include "CoreMinimal.h"
#include "BA_Map.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "RtsWorldMap.generated.h"

class UDataTable;
class UWorldMapWidget;

/**
 * Game-specific ABA_Map subclass. The plugin only gets you from a click to a UV coordinate
 * (ABA_MapPawn::Select is Blueprint-only); everything from there — sampling the province-ID
 * texture, resolving the pixel color against the authored province DataTable, and turning that
 * into a FProvinceId — is gameplay logic, so it lives here in C++ rather than in the map pawn's
 * event graph.
 *
 * ProvinceIdTexture/ProvinceMapData are the same ProvID texture and Color<->Name DataTable
 * (FProvinceMapDataGame rows) already authored for BloodAnvil's LUT shading — set them on the
 * Blueprint subclass placed in the level. UWorldMapWidget is the one interface into the rest of the
 * game this class knows about: it forwards a resolved click there and nothing else — this module
 * does not know RtsUI/UPlayerCharacterViewModel, see UWorldMapWidget's class comment.
 *
 * It also owns what the map looks like when nobody overrides anything: the political map, derived
 * from UProvinces::GetOwningNation and the nation's FNationInfo::MapColor. Overrides are a layer on
 * top of that, never a replacement for it — the LUT has to be written whole (WriteToLUTs clears it),
 * so merging both layers has to happen here rather than in the caller.
 */
UCLASS()
class WORLDMAP_API ARtsWorldMap : public ABA_Map
{
	GENERATED_BODY()

public:
	/** The ProvID texture BloodAnvil's LUT shading reads from — sampled here too, to resolve a
	 * click instead of only to color the map. */
	UPROPERTY(EditAnywhere, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdTexture;

	/** Rows of FProvinceMapDataGame (BA_DataFunctions.h) — same table the LUT writer keys off of.
	 * FProvinceMapDataGame::Name is expected to equal the FProvinceDisplayInfo::MapId authored on
	 * the matching province. */
	UPROPERTY(EditAnywhere, Category="Province Lookup")
	TObjectPtr<UDataTable> ProvinceMapData;

	/** Default shading for a province no nation owns — otherwise it would take the color of the
	 * default-constructed FNationInfo and read as if it belonged to someone. */
	UPROPERTY(EditAnywhere, Category="Shading")
	FColor UnownedProvinceColor = FColor(90, 90, 90);

	/** Registered by UWorldMapWidget itself when it initializes — it resolves this actor via
	 * GetRtsWorldMap, so neither the HUD nor a Blueprint has to wire the two together. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetWorldMapWidget(UWorldMapWidget* InWidget);

	/** Bind this to the map pawn's Select event, passing the UV it already computed. Resolves the
	 * clicked province and reports it to UWorldMapWidget::HandleProvinceClicked — an invalid id
	 * (no matching color) clears the selection the same way clicking off any province does
	 * elsewhere in this codebase. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SelectProvinceAt(FVector2D InUV);

	/** Replaces the override layer and repaints. A province absent from InVisuals is *not* cleared
	 * — it falls back to the default shading (its owning nation's color), so an empty map means
	 * "plain political map", not "black earth". What an override means is entirely up to the
	 * caller; UWorldMapWidget feeds this from UWorldMapViewModel::OverriddenVisuals. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	/** SetProvinceVisuals for callers that only care about the primary color (see
	 * UFoundOrganizationModalWidgetViewModel, coloring by founding eligibility). */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceOverlayColors(const TMap<FProvinceId, FColor>& InColors);

	/** Drops the override layer — every province goes back to its nation's color. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void ClearProvinceOverlayColors();

	/** The map actor of InWorldContextObject's world, so a caller outside WorldMap (RtsUI) can reach
	 * it without depending on the BloodAnvil plugin itself. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map", meta=(WorldContext="WorldContextObject"))
	static ARtsWorldMap* GetRtsWorldMap(const UObject* WorldContextObject);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Writes the whole LUT: every province the ProvID table knows, painted with its override if it
	 * has one and with its nation's color otherwise. */
	void Repaint();

	UFUNCTION()
	void HandleProvincesChanged();

	UFUNCTION()
	void HandleOwningNationChanged(FProvinceId InProvinceId);

	FProvinceId ResolveProvinceAt(FVector2D InUV) const;

	/** Builds ProvinceIdToColorRG on first use — the inverse of ResolveProvinceAt's linear scan,
	 * cheap to keep around since ProvinceMapData never changes after it is authored. */
	void EnsureProvinceColorLookup();

	UPROPERTY(Transient)
	TObjectPtr<UWorldMapWidget> WorldMapWidget;

	/** The override layer, kept so a repaint triggered by the simulation (ownership changed) can
	 * put it back on top of the recomputed political map. */
	TMap<FProvinceId, FProvinceVisuals> OverriddenVisuals;

	/** FProvinceId -> the ProvID pixel color WriteToLUTs keys its rows on. */
	TMap<FProvinceId, FColorRG> ProvinceIdToColorRG;
};
