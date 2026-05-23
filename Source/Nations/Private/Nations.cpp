#include "Nations.h"

#define LOCTEXT_NAMESPACE "FNationsModule"

void FNationsModule::StartupModule()
{
    
}

void FNationsModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FNationsModule, Nations)

UNations::UNations()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);
}