#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RtsPlayer.generated.h"

class ATimeSimulation;

class FRtsPlayerModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRtsPlayerEvent);

UCLASS()
class ARtsPlayer : public APlayerController
{
    GENERATED_BODY()
public:
    ARtsPlayer();

    UPROPERTY(BlueprintAssignable)
    FRtsPlayerEvent OnControlledCharacterChanged;
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetControlledCharacter, Replicated, ReplicatedUsing=OnRep_ControlledCharacter)
    class ARtsCharacter* ControlledCharacter;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetControlledCharacter(ARtsCharacter* NewControlledCharacter)
    {
        ControlledCharacter = NewControlledCharacter;
        OnControlledCharacterChanged.Broadcast();
    }
    
    // Replication Helper
    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_SetSpeed(ATimeSimulation* InTarget, int32 InSpeed);
private:
    UFUNCTION()
    FORCEINLINE void OnRep_ControlledCharacter()
    {
        OnControlledCharacterChanged.Broadcast();
    }
};
