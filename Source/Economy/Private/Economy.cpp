#include "Economy.h"

#define LOCTEXT_NAMESPACE "FEconomyModule"

void FEconomyModule::StartupModule()
{
    
}

void FEconomyModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FEconomyModule, Economy)

UEconomy::UEconomy()
{
	SetIsReplicatedByDefault(true);
}

void UEconomy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
