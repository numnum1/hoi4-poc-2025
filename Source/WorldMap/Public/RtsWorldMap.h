#pragma once

#include "CoreMinimal.h"
#include "BA_Map.h"
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

	/** Pushed in by the HUD once it has created the WeltMap widget. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetWorldMapWidget(UWorldMapWidget* InWidget);

	/** Bind this to the map pawn's Select event, passing the UV it already computed. Resolves the
	 * clicked province and reports it to UWorldMapWidget::HandleProvinceClicked — an invalid id
	 * (no matching color) clears the selection the same way clicking off any province does
	 * elsewhere in this codebase. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SelectProvinceAt(FVector2D InUV);

	/** Colors every province in InColors and clears every other one, via the plugin's LUT shading
	 * (WriteToLUTs). This class only knows how to turn a province->color map into a LUT write —
	 * what the color means is entirely up to the caller (see
	 * UFoundOrganizationModalWidgetViewModel for the current one, coloring by founding
	 * eligibility). Reuses the Primary LUT channel; nothing else writes to it yet. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceOverlayColors(const TMap<FProvinceId, FColor>& InColors);

	UFUNCTION(BlueprintCallable, Category="World Map")
	void ClearProvinceOverlayColors();

	/** ABA_Map::GetMap cast to this game's subclass, so a caller outside WorldMap (RtsUI) can reach
	 * the map without depending on the BloodAnvil plugin itself. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="World Map", meta=(WorldContext="WorldContextObject"))
	static ARtsWorldMap* GetRtsWorldMap(const UObject* WorldContextObject);

private:
	FProvinceId ResolveProvinceAt(FVector2D InUV) const;

	/** Builds ProvinceIdToColorRG on first use — the inverse of ResolveProvinceAt's linear scan,
	 * cheap to keep around since ProvinceMapData never changes after it is authored. */
	void EnsureProvinceColorLookup();

	UPROPERTY(Transient)
	TObjectPtr<UWorldMapWidget> WorldMapWidget;

	/** FProvinceId -> the ProvID pixel color WriteToLUTs keys its rows on. */
	TMap<FProvinceId, FColorRG> ProvinceIdToColorRG;
};
