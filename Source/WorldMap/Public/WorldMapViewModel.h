#pragma once

#include "CoreMinimal.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "WorldMapViewModel.generated.h"

/**
 * How the WeltMap deviates from its default shading: one FProvinceVisuals per province that is
 * overridden, provinces left out keep the political map ARtsWorldMap derives from province
 * ownership. Empty is therefore the normal state, not a blank world. Nothing here is derived from
 * the simulation — the map has no opinion on why a province is green — so this is pushed in from
 * above as a whole map at a time, by whoever owns the current view mode.
 *
 * The view model only holds the colors; turning them into the plugin's LUT write is
 * UWorldMapWidget::ApplyOverriddenVisuals, so this class stays a plain value holder and nothing here
 * needs to know the map actor exists.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class WORLDMAP_API UWorldMapViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Replaces the whole map — a province absent from InOverriddenVisuals loses its override.
	 * Single-parameter so it is a valid MVVM destination binding for a compute method. */
	UFUNCTION(BlueprintCallable, Category="World Map Viewmodel")
	void SetOverriddenVisuals(const TMap<FProvinceId, FProvinceVisuals>& InOverriddenVisuals);

	const TMap<FProvinceId, FProvinceVisuals>& GetOverriddenVisuals() const { return OverriddenVisuals; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="World Map Viewmodel", meta=(AllowPrivateAccess))
	TMap<FProvinceId, FProvinceVisuals> OverriddenVisuals;
};
