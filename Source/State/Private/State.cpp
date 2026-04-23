#include "State.h"

#include "Demography.h"
#include "Economy.h"
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
	StateOwner = nullptr;
	
	Ecology = CreateDefaultSubobject<UEconomy>(TEXT("Economy"));
	Demography = CreateDefaultSubobject<UDemography>(TEXT("Demography"));
}

void AState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AState, StateInfo);
	DOREPLIFETIME(AState, StateOwner);
}
