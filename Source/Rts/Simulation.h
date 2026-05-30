#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Simulation.generated.h"

class FSimulationModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS()
class RTS_API USimulation : public UActorComponent
{
    GENERATED_BODY()
public:
    USimulation();
    
    UFUNCTION(BlueprintCallable)
    void SimulationTick(float InDeltaHours);
};