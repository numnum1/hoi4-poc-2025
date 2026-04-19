#pragma once

#include "CoreMinimal.h"
#include "ScheduleEntry.h"
#include "Modules/ModuleManager.h"
#include "Schedule.generated.h"

class FScheduleModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScheduleEvent);

UCLASS()
class SCHEDULE_API USchedule : public UActorComponent
{
    GENERATED_BODY()
public:
    USchedule();

    UPROPERTY(BlueprintAssignable)
    FScheduleEvent OnEntriesChanged;
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Entries)
    TArray<FScheduleEntry> Entries;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    bool FindEntry(FScheduleEntry& OutEntry, int32 InHour) const;
    
private:
    UFUNCTION()
    FORCEINLINE void OnRep_Entries()
    {
        OnEntriesChanged.Broadcast();
    }
};