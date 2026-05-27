#include "Nations.h"

#include "Net/UnrealNetwork.h"

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

void UNations::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UNations, Infos);
}
