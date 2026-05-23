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


/**
 * 
 */
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class BUILDINGS_API UBuildings : public UActorComponent
{
	GENERATED_BODY()
public:
	UBuildings();
private:
};