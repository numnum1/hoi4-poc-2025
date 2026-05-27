#include "Races.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FRacesModule"

void FRacesModule::StartupModule()
{
    
}

void FRacesModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRacesModule, Races)

URaces::URaces()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);
}

void URaces::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(URaces, Races);
}
