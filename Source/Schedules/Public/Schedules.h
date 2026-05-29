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

UCLASS()
class SCHEDULES_API USchedules : public UActorComponent
{
    GENERATED_BODY()
public:
    USchedules();
    
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FSchedule> Schedules;
    
};