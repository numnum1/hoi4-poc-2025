#include "Demography.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FDemographyModule"

void FDemographyModule::StartupModule()
{
    
}

void FDemographyModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDemographyModule, Demography)

UDemography::UDemography()
{
	SetIsReplicatedByDefault(true);
}

void UDemography::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDemography, Race);
	DOREPLIFETIME(UDemography, Count);
}
