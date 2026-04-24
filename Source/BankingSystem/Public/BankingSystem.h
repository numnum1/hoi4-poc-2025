#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBankingSystemModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
