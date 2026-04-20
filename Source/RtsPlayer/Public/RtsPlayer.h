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

UCLASS()
class ARtsPlayer : public APlayerController
{
    GENERATED_BODY()
public:
    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_SetSpeed(ATimeSimulation* InTarget, int32 InSpeed);
};
