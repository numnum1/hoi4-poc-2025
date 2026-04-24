#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FJobModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct JOB_API FJob
{
	GENERATED_BODY()
};
