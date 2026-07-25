#pragma once

#include "CoreMinimal.h"
#include "Schedule.h"
#include "Modules/ModuleManager.h"
#include "Schedules.generated.h"

class FSchedulesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSchedulesEvent, int32, InScheduleIndex);

UCLASS()
class SCHEDULES_API USchedules : public UActorComponent
{
    GENERATED_BODY()
public:
    USchedules();

    UPROPERTY(BlueprintAssignable)
    FSchedulesEvent OnScheduleChanged;
    
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FSchedule> Schedules;
    
};