#include "Provinces.h"

#define LOCTEXT_NAMESPACE "FProvincesModule"

void FProvincesModule::StartupModule()
{
    
}

void FProvincesModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FProvincesModule, Provinces)

UProvinces::UProvinces()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);
}