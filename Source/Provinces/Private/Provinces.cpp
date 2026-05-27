#include "Provinces.h"

#include "Net/UnrealNetwork.h"

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

void UProvinces::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UProvinces, Infos);
}

void UProvinces::Setup(const TArray<FProvinceInfo>& NewInfos)
{
	Infos = NewInfos;
	
	// Set containers to exact sizes
	MapIds.Empty(Infos.Num());
	OwningNationIndexes.SetNumUninitialized(Infos.Num());
	
	// For each info initialize fitting entry
	for (int I = 0; I < Infos.Num(); ++I)
	{
		MapIds.Emplace(Infos[I].MapId, I);
		OwningNationIndexes[I] = Infos[I].OwningNationIndex;
	}
}

void UProvinces::OnRep_Infos()
{
	Setup(Infos);
}
