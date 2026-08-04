#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BuildingCatalogSettings.generated.h"

class UBuildingType;

/**
 * The project-wide list of building types that exist, authored once in Project Settings rather
 * than per scenario — a building's kind (what it costs, whether it can seat an organization) is a
 * fact about the game, not something a scenario varies the way it varies which characters or
 * organizations start out in play.
 *
 * UBuildings resolves this into its runtime index<->asset registry on first use; see
 * UBuildings::EnsureCatalogLoaded.
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Rts Building Catalog"))
class BUILDINGS_API UBuildingCatalogSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Buildings")
	TArray<TSoftObjectPtr<UBuildingType>> BuildingTypes;
};
