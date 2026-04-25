#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Buildings.generated.h"

class FBuildingsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS(BlueprintType)
class BUILDINGS_API ABuilding : public AActor
{
    GENERATED_BODY()
public:
    ABuilding();
};