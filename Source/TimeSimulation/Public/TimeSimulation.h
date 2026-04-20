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
    UPROPERTY(BlueprintAssignable)
    FTimeSimulationEvent OnSpeedChanged;
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_DateTime)
    FDateTime DateTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetSpeed, Replicated, ReplicatedUsing=OnRep_Speed)
    int32 Speed;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    virtual void Tick(float DeltaSeconds) override;
    
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    void SetSpeed(const int32 InSpeed = 1)
    {
        Speed = InSpeed;
        PrimaryActorTick.TickInterval = 0.25f;
        OnSpeedChanged.Broadcast();
    }
    
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
    UFUNCTION()
    FORCEINLINE void OnRep_Speed()
    {
        OnSpeedChanged.Broadcast();
    }
};
