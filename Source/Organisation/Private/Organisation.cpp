#include "Organisation.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FOrganisationModule"

void FOrganisationModule::StartupModule()
{
    
}

void FOrganisationModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FOrganisationModule, Organisation)

AOrganization::AOrganization()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	
	Info = {};
}

void AOrganization::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AOrganization, Info);
}

void AOrganization::Hire()
{
	
}
