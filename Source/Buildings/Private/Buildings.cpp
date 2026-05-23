#include "Buildings.h"

#define LOCTEXT_NAMESPACE "FBuildingsModule"

void FBuildingsModule::StartupModule()
{
    
}

void FBuildingsModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBuildingsModule, Buildings)

UBuildings::UBuildings()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);
}
