#pragma once

#include "CoreMinimal.h"
#include "Schedule.h"
#include "ScheduleInfo.generated.h"

/**
 * One authored schedule plus the name it is offered under. Produced by
 * ScenarioLoader::ExtractSchedules from the UScheduleAsset a character references, held by
 * USchedules as the runtime library.
 *
 * The name lives here and not in FSchedule on purpose: FSchedule goes into
 * FCharacterScheduleSharedFragment, and FText has no business in a Mass fragment (same rule
 * that keeps FCharacterDisplayInfo out of the character entity).
 */
USTRUCT(BlueprintType)
struct SCHEDULES_API FScheduleInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ScheduleName = FText::FromString("Unnamed Schedule");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSchedule Schedule;
};
