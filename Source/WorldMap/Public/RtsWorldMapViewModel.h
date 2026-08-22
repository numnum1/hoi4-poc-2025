#pragma once

#include "CoreMinimal.h"
#include "MapTypes.h"
#include "MVVMViewModelBase.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "RtsWorldMapViewModel.generated.h"

class ARtsWorldMap;
class UWorldMapSubsystem;

/**
 * What the world map shows. Owned by UWorldMapWidget, which creates it and pushes into it.
 *
 * It is the only place province ids and pixels meet: colors and nations arrive keyed by FProvinceId
 * from RtsUI, get translated through UWorldMapSubsystem, and reach ARtsWorldMap as pixel-keyed rows
 * the actor can write without knowing what a province is.
 *
 * It holds rendering state and nothing else. Which province the player selected, why a click
 * mattered, which window it opened — none of that lives here; it is settled in RtsUI, which this
 * module must not depend on.
 *
 * It has no dispatchers, per the CreateInstance rule in CLAUDE.md: it has no parent view model that
 * could bind one. Clicks leave through UWorldMapWidget.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class WORLDMAP_API URtsWorldMapViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Rts World Map ViewModel")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	UFUNCTION(BlueprintCallable, Category="Rts World Map ViewModel")
	void SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData);

private:
	ARtsWorldMap* ResolveWorldMap();

	TArray<FWorldMapLabel> BuildLabels(const UWorldMapSubsystem& InSubsystem) const;

	static bool BuildLabelForZone(
		const UWorldMapSubsystem& InSubsystem,
		const FMapNameZone& InZone,
		const FText& InName,
		const TArray<FNationId>& InTraceOverTags,
		FWorldMapLabel& OutLabel);

	TMap<FProvinceId, FProvinceVisuals> ProvinceVisuals;

	TMap<FNationId, FWorldMapNationData> NationData;

	UPROPERTY(Transient)
	TObjectPtr<ARtsWorldMap> WorldMap;

	bool bMissingMapLogged = false;

	bool bNoMatchingProvincesLogged = false;
};
