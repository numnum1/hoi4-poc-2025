#pragma once

#include "CoreMinimal.h"
#include "BuildingCategory.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Styling/SlateBrush.h"
#include "RtsBuildingCategories.generated.h"

/**
 * The build menu's section catalogue, hardcoded as small switches over EBuildingCategory rather
 * than a data asset or a registry — the same shape URtsMapModes has for map modes. A new section
 * means one EBuildingCategory entry and one case here.
 *
 * Which building kinds fall into a section is not answered here: that is authored on the
 * UBuildingType assets, and the grouping happens in UBuildMenuWidgetViewModel.
 */
UCLASS()
class RTSUI_API URtsBuildingCategories : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Every category, in the order the build menu should list its sections. */
	UFUNCTION(BlueprintPure, Category="Rts Building Categories")
	static TArray<EBuildingCategory> GetCategories();

	static FText GetDisplayName(EBuildingCategory InCategory);
	static FText GetDescription(EBuildingCategory InCategory);
	static FSlateBrush GetIcon(EBuildingCategory InCategory);
};
