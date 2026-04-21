#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Flag.generated.h"

class FFlagModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct FFlag
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlateBrush PlaceholderBrush = FSlateNoResource();
};