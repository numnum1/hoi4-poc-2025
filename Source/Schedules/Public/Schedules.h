#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Schedule.h"
#include "ScheduleInfo.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Schedules.generated.h"

class FSchedulesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

/**
 * The runtime library of authored schedules — the set a character can be assigned to, addressed
 * by FScheduleId. Filled once at scenario load from the UScheduleAssets the characters
 * reference, then read-only, the same way URaces holds its authored race data.
 *
 * This is not per-character state: the schedule a character actually runs is a copy in its
 * FCharacterScheduleSharedFragment, reachable through UCharacters::GetSchedule. The library
 * exists so the UI can name that schedule and offer the others.
 */
UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class SCHEDULES_API USchedules : public UActorComponent
{
    GENERATED_BODY()
public:
    USchedules();

    /** Replaces the whole library. Called once per scenario load. */
    void Setup(TArray<FScheduleInfo> InSchedules);

    UFUNCTION(BlueprintPure)
    int32 Num() const { return Schedules.Num(); }

    UFUNCTION(BlueprintPure)
    bool IsValidSchedule(FScheduleId InScheduleId) const { return Schedules.IsValidIndex(InScheduleId.Value); }

    UFUNCTION(BlueprintPure)
    FSchedule GetSchedule(FScheduleId InScheduleId) const;

    UFUNCTION(BlueprintPure)
    FText GetScheduleName(FScheduleId InScheduleId) const;

    UFUNCTION(BlueprintPure)
    FScheduleInfo GetScheduleInfo(FScheduleId InScheduleId) const;

    FORCEINLINE const TArray<FScheduleInfo>& GetScheduleInfos() const { return Schedules; }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FScheduleInfo> Schedules;
};
