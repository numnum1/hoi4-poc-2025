#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "WorldMapWidget.generated.h"

class ARtsMapPawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldMapProvinceClicked, FProvinceId, ProvinceId);

/**
 * The world map's interface to the rest of the UI, and nothing else: clicks leave through
 * OnProvinceClicked, colors arrive through SetProvinceVisuals. It exists so the map can be dropped
 * anywhere in the HUD's widget tree without whoever places it having to know about the map actor or
 * the pawn.
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
	/** Fires for every click on the map; an invalid id means the player clicked off any province. */
	UPROPERTY(BlueprintAssignable, Category="World Map")
	FWorldMapProvinceClicked OnProvinceClicked;

	/** Paints the given provinces. Provinces left out keep whatever color they had — this is a
	 * change set, not a whole world, so the caller decides what "reset" means. */
	UFUNCTION(BlueprintCallable, Category="World Map")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

protected:
	/** Bound here rather than in NativeOnInitialized: the pawn has to exist and be possessed first,
	 * which is not guaranteed while the widget is still being constructed. */
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleProvinceClicked(FProvinceId InProvinceId);

	UPROPERTY(Transient)
	TObjectPtr<ARtsMapPawn> MapPawn;
};
