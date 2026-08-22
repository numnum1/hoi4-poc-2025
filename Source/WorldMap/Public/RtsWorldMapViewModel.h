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
	/** The color of every province that should change. Provinces left out keep what they had — this
	 * is a change set, not a whole world, so the caller decides what "reset" means. */
	UFUNCTION(BlueprintCallable, Category="Rts World Map ViewModel")
	void SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals);

	/** Who owns what and what they are called. Both the map's country labels and the ownership
	 * lookup its label Blueprint traces along are rebuilt from this. */
	UFUNCTION(BlueprintCallable, Category="Rts World Map ViewModel")
	void SetNationData(const TMap<FNationId, FWorldMapNationData>& InNationData);

private:
	/** The map actor, found once and kept. Retried rather than resolved up front, because the view
	 * model can be created before the level's actors are up. */
	ARtsWorldMap* ResolveWorldMap();

	/** One label per contiguous zone a nation holds, laid out along the zone's border provinces.
	 * Points come out as UVs — the globe's radius is the actor's business. */
	TArray<FWorldMapLabel> BuildLabels(const UWorldMapSubsystem& InSubsystem) const;

	/** The three points one label is laid out along: the western of the two furthest-apart border
	 * provinces, the zone's centre, and the eastern one. False for a zone whose provinces have no
	 * unit position to place anything at. */
	static bool BuildLabelForZone(
		const UWorldMapSubsystem& InSubsystem,
		const FMapNameZone& InZone,
		const FText& InName,
		const TArray<FNationId>& InTraceOverTags,
		FWorldMapLabel& OutLabel);

	/** The last state pushed in — what the map is currently showing. */
	TMap<FProvinceId, FProvinceVisuals> ProvinceVisuals;

	TMap<FNationId, FWorldMapNationData> NationData;

	UPROPERTY(Transient)
	TObjectPtr<ARtsWorldMap> WorldMap;

	/** So a level without a map actor is reported once, not on every repaint. */
	bool bMissingMapLogged = false;

	/** Likewise for a push whose provinces the map table does not list at all — the usual sign of a
	 * scenario and a province-ID table that do not belong together. */
	bool bNoMatchingProvincesLogged = false;
};
