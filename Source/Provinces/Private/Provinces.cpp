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

void UProvinces::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	InitValuesFromInfos();	
}

void UProvinces::InitValuesFromInfos()
{
	// Set containers to exact sizes
	MapIds.Empty(Infos.Num());
	OwningNations.SetNumUninitialized(Infos.Num());
	
	// For each info initialize fitting entry
	for (int I = 0; I < Infos.Num(); ++I)
	{
		MapIds.Emplace(Infos[I].MapId, I);
		OwningNations[I] = Infos[I].OwningNationId;
	}
}
