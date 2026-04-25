#include "Organisation.h"

#include "OrganizationPosition.h"
#include "PositionHolder.h"
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
	PositionHolders = {};
}

void AOrganization::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AOrganization, Info);
	DOREPLIFETIME(AOrganization, PositionHolders);
}

void AOrganization::BuyProperty()
{
	for (AActor* PositionHolder : PositionHolders)
	{
		verify(PositionHolder);
		verify(PositionHolder->Implements<UPositionHolder>());
		
		IPositionHolder::Execute_BuyProperty(PositionHolder, this);
	}
}

void AOrganization::RegisterNewWorker(AActor* InNewPositionHolder)
{
	verify(InNewPositionHolder);
	verify(InNewPositionHolder->Implements<UPositionHolder>());
	
	ensure(!PositionHolders.Contains(InNewPositionHolder));
	
	PositionHolders.AddUnique(InNewPositionHolder);
}
