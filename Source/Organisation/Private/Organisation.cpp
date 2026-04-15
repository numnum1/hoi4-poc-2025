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

AOrganisation::AOrganisation()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AOrganisation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AOrganisation, Info);
}
