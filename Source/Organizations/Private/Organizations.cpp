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
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	Infos = {};
}

int32 UOrganizations::Add(const FOrganizationInfo& NewOrg)
{
	Infos.Add(NewOrg);
	SettledNations.Add({});
	Alignments.Add({});
	return Infos.Num() - 1;
}

void UOrganizations::PublishAlignment(const int32 InOrganizationIndex, const TArray<float>& InAlignment)
{
	Alignments[InOrganizationIndex].bPublished = true;
	Alignments[InOrganizationIndex].Branches = InAlignment;
}

void UOrganizations::SettleNation(const int32 InOrganizationIndex, const int32 InNationIndex)
{
	check(!SettledNations[InOrganizationIndex].Value.Contains(InNationIndex));
	
	SettledNations[InOrganizationIndex].Value.Add(InNationIndex);
	OnNationSettled.Broadcast(InOrganizationIndex);
}
