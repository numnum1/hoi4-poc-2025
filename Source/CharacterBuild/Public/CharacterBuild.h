#pragma once

#include "CoreMinimal.h"
#include "JobLevel.h"
#include "RacialLevel.h"
#include "Modules/ModuleManager.h"
#include "CharacterBuild.generated.h"


class FCharacterBuildModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS()
class UCharacterBuild : public UActorComponent
{
    GENERATED_BODY()
public:
    UCharacterBuild();
private:
    // Levels
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
    TArray<FRacialLevel> RacialLevels;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
    TArray<FJobLevel> JobLevels;
    // Computations
};
