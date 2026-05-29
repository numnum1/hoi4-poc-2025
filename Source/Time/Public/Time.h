#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Time.generated.h"

class FTimeModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimeEvent);

UCLASS(BlueprintType)
class TIME_API UTime : public UActorComponent
{
    GENERATED_BODY()
public:
    UTime();

    UPROPERTY(BlueprintAssignable)
    FTimeEvent OnCurrentTimeChanged;
    UPROPERTY(BlueprintAssignable)
    FTimeEvent OnSpeedChanged;
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_CurrentTime)
    FDateTime CurrentTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Speed, BlueprintSetter=SetSpeed)
    int32 Speed;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    FORCEINLINE const FDateTime& GetCurrentTime() const { return CurrentTime; }
    
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetSpeed(const int32 NewSpeed)
    {
        Speed = NewSpeed;
        OnSpeedChanged.Broadcast();
    }
protected:
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
    UFUNCTION()
    FORCEINLINE void OnRep_CurrentTime()
    {
        OnCurrentTimeChanged.Broadcast();
    }
    UFUNCTION()
    FORCEINLINE void OnRep_Speed()
    {
        OnSpeedChanged.Broadcast();
    }
    
    void UpdateTime(float DeltaTime);
};