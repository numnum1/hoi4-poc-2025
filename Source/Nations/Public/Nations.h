#pragma once

#include "CoreMinimal.h"
#include "NationInfo.h"
#include "Modules/ModuleManager.h"
#include "Nations.generated.h"

class FNationsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class NATIONS_API UNations : public UActorComponent
{
    GENERATED_BODY()
public:
    UNations();
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FNationInfo> Infos;
};
