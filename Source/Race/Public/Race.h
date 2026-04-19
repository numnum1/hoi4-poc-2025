#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Race.generated.h"

class FRaceModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

/**
 * 
 */
UCLASS(BlueprintType)
class RACE_API URace : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    URace();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText RaceName;
};