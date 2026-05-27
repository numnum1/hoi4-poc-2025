#include "Organizations.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FOrganizationsModule"

void FOrganizationsModule::StartupModule()
{
    
}

void FOrganizationsModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FOrganizationsModule, Organizations)

UOrganizations::UOrganizations()
{
	Infos = {};
}

void UOrganizations::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UOrganizations, Infos);
	DOREPLIFETIME(UOrganizations, SettledNations);
}

int32 UOrganizations::Add(const FOrganizationInfo& NewOrg)
{
	Infos.Add(NewOrg);
	SettledNations.Add({});
	return Infos.Num() - 1;
}
