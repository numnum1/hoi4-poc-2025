#pragma once

#include "CoreMinimal.h"
#include "Genome.h"
#include "SkillSet.h"
#include "Modules/ModuleManager.h"
#include "Race.generated.h"

class URaceTrait;
class USpecies;

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
class RACE_API URace : public UGenome
{
    GENERATED_BODY()
public:
    URace();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USpecies* Species;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText RaceName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillSet BaseSkills;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<URaceTrait*> Traits;
    
};