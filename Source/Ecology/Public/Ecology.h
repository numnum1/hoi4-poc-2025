#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Ecology.generated.h"

class FEcologyModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

/**
 * 
 */
UCLASS(BlueprintType)
class ECOLOGY_API UEcology : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UEcology();
private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
    FText EcologyName;
};
