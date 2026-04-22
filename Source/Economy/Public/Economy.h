#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Economy.generated.h"

class FEconomyModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS(BlueprintType)
class ECONOMY_API UEconomy : public UActorComponent
{
    GENERATED_BODY()
public:
    UEconomy();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};