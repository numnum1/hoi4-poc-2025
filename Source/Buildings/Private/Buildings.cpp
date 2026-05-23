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
	
	InitWithProvinceAmount();
}

void UBuildings::InitWithProvinceAmount(const int32 InProvinceAmount)
{
	verify(InProvinceAmount >= 0);
	static_assert(MAX_BUILDINGS_PER_PROVINCE >= 0);
	
	BuildingPool.Init({}, InProvinceAmount * MAX_BUILDINGS_PER_PROVINCE);
}
