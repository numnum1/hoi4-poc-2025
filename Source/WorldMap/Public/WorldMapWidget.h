#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapTypes.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "WorldMapWidget.generated.h"

class ARtsWorldMap;
class URtsWorldMapViewModel;

/**
 * The world map's interface to the rest of the UI, and nothing else: colors and nations arrive
 * through the setters, clicks leave through OnProvinceClicked. It exists so the map can be dropped
 * anywhere in the HUD's widget tree without whoever places it having to know about the map actor,
 * the subsystem or the camera pawn.
 *
 * It owns URtsWorldMapViewModel the same way UPlayedCharacterView owns UPlayerCharacterViewModel:
 * created in NativeOnInitialized, handed everything that is pushed in, and exposed through a getter
 * so the Blueprint binds its own widgets to it with creation type "Property Path".
 *
 * It decides nothing. It does not know what a province click means, and it has no opinion on which
 * colors are correct — both are settled in RtsUI, which owns the map mode and pushes the result
 * down here. A C++ dependency in the other direction would make WorldMap depend on RtsUI while
 * RtsUI already depends on WorldMap, a cycle.
 */
UCLASS(Abstract)
class WORLDMAP_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapProvinceClicked OnProvinceClicked;

	/** Forwarded from ARtsWorldMap, which resolves and remembers it. The map highlights the province
	 * on its own - this is only for whoever wants to say something about it. */
	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapProvinceHovered OnHoveredProvinceIdChanged;

	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData);

	/** Passed straight to the view model, which owns the switch. Here so a HUD Blueprint can reach it
	 * without knowing about the view model or the map actor. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetHoverHighlightEnabled(bool bInEnabled);

	UFUNCTION(BlueprintPure, Category="World Map")
	URtsWorldMapViewModel* GetWorldMapViewModel() const { return WorldMapViewModel; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandlePixelClicked(FMapPixelId InPixelId);

	UFUNCTION()
	void HandleHoveredProvinceIdChanged(FProvinceId InProvinceId);

	ARtsWorldMap* ResolveWorldMap();

	UPROPERTY(Transient)
	TObjectPtr<URtsWorldMapViewModel> WorldMapViewModel;

	UPROPERTY(Transient)
	TObjectPtr<ARtsWorldMap> WorldMap;

	bool bMissingMapLogged = false;
};
