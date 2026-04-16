#include "Nation.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FNationModule"

void FNationModule::StartupModule()
{
    
}

void FNationModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FNationModule, Nation)

ANation::ANation()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	
	Info = {};
	States = {};
}

void ANation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANation, Info);
	DOREPLIFETIME(ANation, States);
}
