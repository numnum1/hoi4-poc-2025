#pragma once

#include "CoreMinimal.h"
#include "BuildingKind.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BuildingKinds.generated.h"

/**
 * The building catalogue, hardcoded as a set of small switches over EBuildingKind rather than a
 * data asset per kind. Adding a building means adding one EBuildingKind entry and one case to each
 * switch below — nothing else.
 *
 * Deliberately not a UDataAsset: build cost and "is this a headquarters" are gameplay rules, and
 * rules stay in code where they can be read and reviewed. Assets configure data, never behaviour
 * (see CLAUDE.md). URtsActions is the same shape for actions.
 */
UCLASS()
class BUILDINGS_API UBuildingKinds : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** The kinds that can seat an organization — the list the found-organization modal offers. */
	UFUNCTION(BlueprintPure, Category="Building Kinds")
	static TArray<EBuildingKind> GetHeadquarterKinds();

	/** A headquarters cannot be torn down: UBuildings::Destroy refuses it, so an organization can
	 * never end up without the seat it was founded with. */
	UFUNCTION(BlueprintPure, Category="Building Kinds")
	static bool IsHeadquarters(EBuildingKind InKind);

	UFUNCTION(BlueprintPure, Category="Building Kinds")
	static FText GetDisplayName(EBuildingKind InKind);

	UFUNCTION(BlueprintPure, Category="Building Kinds")
	static FSlateBrush GetIcon(EBuildingKind InKind);

	/** What putting one up costs the character who orders it. */
	UFUNCTION(BlueprintPure, Category="Building Kinds")
	static int32 GetBuildCost(EBuildingKind InKind);
};
