#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RtsIds.h"
#include "WorldMapWidget.generated.h"

class UWorldMapViewModel;

/**
 * The C++ half of the WeltMap widget. Owns the module's UWorldMapViewModel — created itself in
 * NativeOnInitialized, the same "Widgets sind C++-Klassen" root pattern UPlayedCharacterView uses
 * for UPlayerCharacterViewModel — so other CreateInstance view models (a province detail panel, or
 * RtsUI's UPlayerCharacterViewModel::SelectProvince) can bind a Destination Binding to
 * GetWorldMapViewModel()::SelectedProvinceId without this module knowing who reads it.
 *
 * This module deliberately does not know RtsUI/UPlayerCharacterViewModel: what a province
 * selection is supposed to *do* in the player's UI (open PM_ViewProvince, feed the
 * found-organization form's site pick, ...) is wired from the RtsUI side via an MVVM Destination
 * Binding onto SelectProvince, the same "Kind-ViewModels: Create Instance + Initialize" shape
 * UTimezoneViewModel::Initialize is bound with — not a C++ push, which would make WorldMap depend
 * on RtsUI while RtsUI already depends on WorldMap for this same class, a cycle.
 */
UCLASS(Abstract)
class WORLDMAP_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="World Map")
	UWorldMapViewModel* GetWorldMapViewModel() const { return WorldMapViewModel; }

	/** Bind this to the map pawn's province click resolution (ProvID texture -> DataTable name ->
	 * UProvinces::FindByMapId). Passing an invalid id clears the selection. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void HandleProvinceClicked(FProvinceId InProvinceId);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UWorldMapViewModel> WorldMapViewModel;
};
