#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RoleResource.generated.h"

class FRoleResourceModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS(BlueprintType)
class URoleResource : public UDataAsset
{
    GENERATED_BODY()
public:
    URoleResource();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ResourceName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlateBrush Icon;
};