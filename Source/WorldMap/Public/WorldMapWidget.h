#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldNotificationId.h"
#include "RtsIds.h"
#include "WorldMapWidget.generated.h"

class UWorldMapViewModel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldMapProvinceClicked, FProvinceId, ProvinceId);

/**
 * The C++ half of the WeltMap widget. Owns the module's UWorldMapViewModel — created itself in
 * NativeOnInitialized, the same "Widgets sind C++-Klassen" root pattern UPlayedCharacterView uses
 * for UPlayerCharacterViewModel — and is the one place that translates it into the map actor:
 * ApplyOverriddenVisuals turns the view model's province colors into ARtsWorldMap's LUT write, and
 * runs itself whenever OverriddenVisuals changes, so whoever fills the view model never touches
 * ARtsWorldMap.
 *
 * This module deliberately does not know RtsUI/URtsHUDViewModel: what a province click means and
 * which colors the map should show are decided above this widget. Clicks leave through
 * OnProvinceClicked and colors arrive through the view model — a C++ push in either direction would
 * make WorldMap depend on RtsUI while RtsUI already depends on WorldMap for this same class, a
 * cycle.
 */
UCLASS(Abstract)
class WORLDMAP_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="World Map")
	UWorldMapViewModel* GetWorldMapViewModel() const { return WorldMapViewModel; }

	/** Fires for every click the map resolves; an invalid id means the player clicked off any
	 * province. Bound by whoever owns this widget (the HUD), which decides what a selection does. */
	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapProvinceClicked OnProvinceClicked;

	/** Bind this to the map pawn's province click resolution (ProvID texture -> DataTable name ->
	 * UProvinces::FindByMapId). Passing an invalid id clears the selection. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void HandleProvinceClicked(FProvinceId InProvinceId);

	/** Paints the map with the view model's current OverriddenVisuals. Runs automatically on every
	 * change; public for the first paint, when the map actor may not have existed yet. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void ApplyOverriddenVisuals();

protected:
	virtual void NativeOnInitialized() override;

private:
	void HandleOverriddenVisualsChanged(UObject* InViewModel, UE::FieldNotification::FFieldId InFieldId);

	UPROPERTY(Transient)
	TObjectPtr<UWorldMapViewModel> WorldMapViewModel;
};
