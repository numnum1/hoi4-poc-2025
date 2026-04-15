#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UObject/Interface.h"
#include "JuristicPerson.generated.h"

class FJuristicPersonModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UINTERFACE()
class UJuristicPerson : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class JURISTICPERSON_API IJuristicPerson
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void FoundOrganisation();
};

