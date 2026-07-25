#include "Organizations.h"

#include "OrganizationEntitySubsystem.h"
#include "Engine/World.h"

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
}

UOrganizationEntitySubsystem* UOrganizations::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UOrganizationEntitySubsystem>() : nullptr;
}

FOrganizationInfo UOrganizations::GetOrganizationInfo(const int32 InIndex) const
{
	return Infos.IsValidIndex(InIndex) ? Infos[InIndex] : FOrganizationInfo();
}

FOrganizationAlignment UOrganizations::GetAlignment(const int32 InOrganizationIndex) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetAlignment(InOrganizationIndex) : FOrganizationAlignment();
}

TArray<int32> UOrganizations::GetSettledNations(const int32 InOrganizationIndex) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetSettledNations(InOrganizationIndex) : TArray<int32>();
}

int32 UOrganizations::Add(const FOrganizationInfo& NewOrg)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return INDEX_NONE;
	}

	// The entity claims the index; the display row must line up with it.
	const int32 NewIndex = Entities->CreateOrganization();
	check(NewIndex == Infos.Num());
	Infos.Add(NewOrg);

	OnOrganizationAdded.Broadcast();
	return NewIndex;
}

void UOrganizations::PublishAlignment(const int32 InOrganizationIndex, const TArray<float>& InAlignment)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->PublishAlignment(InOrganizationIndex, InAlignment);
	OnAlignmentPublished.Broadcast(InOrganizationIndex);
}

void UOrganizations::SettleNation(const int32 InOrganizationIndex, const int32 InNationIndex)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	if (Entities->SettleNation(InOrganizationIndex, InNationIndex))
	{
		OnNationSettled.Broadcast(InOrganizationIndex);
	}
}
