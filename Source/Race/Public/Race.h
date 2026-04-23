#pragma once

#include "CoreMinimal.h"
#include "SkillSet.h"
#include "Modules/ModuleManager.h"
#include "Race.generated.h"

class URaceCategory;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    URaceCategory* Category;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillSet BaseSkills;
    
};