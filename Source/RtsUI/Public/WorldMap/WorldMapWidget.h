#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RtsIds.h"
#include "WorldMapWidget.generated.h"

class UPlayerCharacterViewModel;
class UWorldMapViewModel;

/**
 * The C++ half of the WeltMap widget. Owns the WorldMap module's UWorldMapViewModel — created
 * itself in NativeOnInitialized, the same "Widgets sind C++-Klassen" root pattern
 * UPlayedCharacterView uses for UPlayerCharacterViewModel — so other CreateInstance view models
 * (e.g. a province detail panel) can bind a Destination Binding to
 * GetWorldMapViewModel()::SelectedProvinceId without caring who set it.
 *
 * It is also the way a click gets *out* of the WorldMap module. UWorldMapViewModel lives in
 * WorldMap and may not know UPlayerCharacterViewModel/RtsUI at all (WorldMap only depends on
 * RtsCore + ModelViewViewModel), so this class holds both sides: whatever drives the map actor
 * (the BloodAnvil map pawn's Blueprint) calls HandleProvinceClicked here, and it updates the pure
 * map selection state and forwards to the played character's view model for everything that
 * selection is supposed to *do* (open PM_ViewProvince, feed the found-organization form's site
 * pick, ...) — same mediator shape as UProvinceDetailWidget::ViewOrganization.
 */
UCLASS(Abstract)
class RTSUI_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="World Map")
	UWorldMapViewModel* GetWorldMapViewModel() const { return WorldMapViewModel; }

	/** Pushed in by the owning view once it has its view model. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel);

	UFUNCTION(BlueprintPure, Category="World Map")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

	/** Bind this to the map pawn's province click resolution (ProvID texture -> DataTable name ->
	 * UProvinces::FindByMapId). Passing an invalid id clears the selection. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void HandleProvinceClicked(FProvinceId InProvinceId);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UWorldMapViewModel> WorldMapViewModel;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;
};
