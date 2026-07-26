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
/** Fired on the server once per crossed game hour. InHoursElapsed is >= 1 and may be > 1 at high Speed. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimeHourEvent, int32, InHoursElapsed);

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
    UPROPERTY(BlueprintAssignable)
    FTimeHourEvent OnHourElapsed;
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    FDateTime CurrentTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetSpeed)
    int32 Speed;
    /** Start of the last game hour we broadcast for. */
    FDateTime LastHourBoundary;
public:
    FORCEINLINE const FDateTime& GetCurrentTime() const { return CurrentTime; }

    UFUNCTION(BlueprintSetter)
    FORCEINLINE void SetSpeed(const int32 NewSpeed)
    {
        Speed = NewSpeed;
        OnSpeedChanged.Broadcast();
    }
protected:
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
    void UpdateTime(float DeltaTime);

    /** Drops minutes/seconds so hour boundaries can be compared directly. */
    static FDateTime TruncateToHour(const FDateTime& In);
};