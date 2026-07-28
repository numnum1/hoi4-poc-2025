#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Modules/ModuleManager.h"
#include "RtsPlayer.generated.h"

class FRtsPlayerModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRtsPlayerEvent);

UCLASS(Blueprintable, Abstract)
class RTSPLAYER_API ARtsPlayer : public APlayerController
{
    GENERATED_BODY()
public:
    ARtsPlayer();

    UPROPERTY(BlueprintAssignable)
    FRtsPlayerEvent OnControlledCharacterIdChanged;
    
private:
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true), BlueprintSetter=SetControlledCharacterId)
    FCharacterId ControlledCharacterId;
public:
    FORCEINLINE const FCharacterId& GetControlledCharacterId() const { return ControlledCharacterId; }
    UFUNCTION(BlueprintSetter)
    FORCEINLINE void SetControlledCharacterId(const FCharacterId NewControlledCharacterId)
    {
        ControlledCharacterId = NewControlledCharacterId;
        OnControlledCharacterIdChanged.Broadcast();
    }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
    /** Drops the controlled character reference if it is the one that just died. */
    UFUNCTION()
    void HandleControlledCharacterDestroyed(FCharacterId InCharacterId);
};