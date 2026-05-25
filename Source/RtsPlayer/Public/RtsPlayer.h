#pragma once

#include "CoreMinimal.h"
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
class ARtsPlayer : public APlayerController
{
    GENERATED_BODY()
public:
    ARtsPlayer();

    UPROPERTY(BlueprintAssignable)
    FRtsPlayerEvent OnControlledCharacterIdChanged;
    
private:
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (AllowPrivateAccess = true), Replicated, ReplicatedUsing=OnRep_ControlledCharacterId, BlueprintSetter=SetControlledCharacterId)
    FGuid ControlledCharacterId;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    FORCEINLINE const FGuid& GetControlledCharacterId() const { return ControlledCharacterId; }
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetControlledCharacterId(const FGuid& NewControlledCharacterId)
    {
        ControlledCharacterId = NewControlledCharacterId;
        OnControlledCharacterIdChanged.Broadcast();
    }
private:
    UFUNCTION()
    FORCEINLINE void OnRep_ControlledCharacterId()
    {
        OnControlledCharacterIdChanged.Broadcast();
    }
};