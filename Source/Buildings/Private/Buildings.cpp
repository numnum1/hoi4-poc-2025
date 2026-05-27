#include "Buildings.h"

#include "Net/UnrealNetwork.h"

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

void UBuildings::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UBuildings, BuildingPool);
	DOREPLIFETIME(UBuildings, BuildingCounts);
	DOREPLIFETIME(UBuildings, BuildingOwners);
}

void UBuildings::Add(UBuildingType* InBuildingType, const int32 InProvinceIndex, const FBuildingOwner& InOwner)
{
	BuildingPool[InProvinceIndex * MAX_BUILDINGS_PER_PROVINCE + BuildingCounts[InProvinceIndex]] = InBuildingType;
	BuildingOwners[InProvinceIndex * MAX_BUILDINGS_PER_PROVINCE + BuildingCounts[InProvinceIndex]] = InOwner;
	BuildingCounts[InProvinceIndex]++;
}

void UBuildings::InitWithProvinceAmount(const int32 InProvinceAmount)
{
	verify(InProvinceAmount >= 0);
	static_assert(MAX_BUILDINGS_PER_PROVINCE >= 0);
	
	BuildingPool.Init({}, InProvinceAmount * MAX_BUILDINGS_PER_PROVINCE);
	BuildingCounts.Init({}, InProvinceAmount);
	BuildingOwners.Init({}, InProvinceAmount * MAX_BUILDINGS_PER_PROVINCE);
}
