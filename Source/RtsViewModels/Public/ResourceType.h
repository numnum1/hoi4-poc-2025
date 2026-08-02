#pragma once

#include "CoreMinimal.h"
#include "ResourceType.generated.h"

/**
 * Every resource that exists, hardcoded — there is no per-resource UObject subclass. URtsResources
 * switches over this to answer everything about one resource (which details it has, display data,
 * the amount itself); UResourceViewModel just carries the result of that switch as UI data.
 */
UENUM(BlueprintType)
enum class EResourceType : uint8
{
	RT_Money    UMETA(DisplayName = "Money"),
	RT_Manpower UMETA(DisplayName = "Manpower"),
	RT_Crystals UMETA(DisplayName = "Crystals"),
};

/**
 * Every sub-value any resource can show, in one flat enum rather than one enum per resource: the
 * resource row renders its details generically, so it must be able to name them all. Which of
 * these a given resource actually offers is URtsResources::GetDetails' switch.
 */
UENUM(BlueprintType)
enum class EResourceDetail : uint8
{
	RD_Total       UMETA(DisplayName = "Total"),
	RD_Credit      UMETA(DisplayName = "Credit"),
	RD_Outstanding UMETA(DisplayName = "Outstanding"),
	RD_Used        UMETA(DisplayName = "Used"),
	RD_Free        UMETA(DisplayName = "Free"),
};
