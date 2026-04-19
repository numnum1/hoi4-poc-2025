#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "TimeSimulation.generated.h"

class FTimeSimulationModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimeSimulationEvent);

UCLASS(BlueprintType)
class TIMESIMULATION_API ATimeSimulation : public AInfo
{
    GENERATED_BODY()
public:
    ATimeSimulation();

    UPROPERTY(BlueprintAssignable)
    FTimeSimulationEvent OnDateTimeChanged;
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_DateTime)
    FDateTime DateTime;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    virtual void Tick(float DeltaSeconds) override;
    
    FORCEINLINE const FDateTime& GetDateTime()
    {
        return DateTime;
    }
private:
    UFUNCTION()
    FORCEINLINE void OnRep_DateTime()
    {
        OnDateTimeChanged.Broadcast();
    }
};
