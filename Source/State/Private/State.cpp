#include "State.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FStateModule"

void FStateModule::StartupModule()
{
    
}

void FStateModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FStateModule, State)

AState::AState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	
	StateInfo = {};
	Timezone = nullptr;
}

void AState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AState, StateInfo);
}
