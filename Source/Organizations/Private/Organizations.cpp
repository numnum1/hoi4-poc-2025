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

bool UOrganizations::IsAlive(const FOrganizationId InOrganizationId) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InOrganizationId);
}

FOrganizationInfo UOrganizations::GetOrganizationInfo(const FOrganizationId InOrganizationId) const
{
	return Infos.IsValidIndex(InOrganizationId.Value) ? Infos[InOrganizationId.Value] : FOrganizationInfo();
}

FCharacterId UOrganizations::GetOwnerCharacter(const FOrganizationId InOrganizationId) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOwnerCharacter(InOrganizationId) : FCharacterId();
}

bool UOrganizations::IsOwnedBy(const FOrganizationId InOrganizationId, const FCharacterId InCharacterId) const
{
	// An ownerless organization must not read as owned by an ownerless character.
	return InCharacterId.IsValid() && GetOwnerCharacter(InOrganizationId) == InCharacterId;
}

FBuildingId UOrganizations::GetHeadquarters(const FOrganizationId InOrganizationId) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetHeadquarters(InOrganizationId) : FBuildingId();
}

FOrganizationAlignment UOrganizations::GetAlignment(const FOrganizationId InOrganizationId) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetAlignment(InOrganizationId) : FOrganizationAlignment();
}

TArray<FNationId> UOrganizations::GetSettledNations(const FOrganizationId InOrganizationId) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetSettledNations(InOrganizationId) : TArray<FNationId>();
}

TArray<FOrganizationId> UOrganizations::CollectOrganizationsSettledIn(const FNationId InNationId) const
{
	const UOrganizationEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectOrganizationsSettledIn(InNationId) : TArray<FOrganizationId>();
}

FOrganizationId UOrganizations::Add(const FOrganizationInfo& NewOrg, const FCharacterId InFounderId)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return FOrganizationId();
	}

	// The entity claims the id; the display row must line up with it.
	const FOrganizationId NewId = Entities->CreateOrganization(InFounderId);
	check(NewId.Value == Infos.Num());
	Infos.Add(NewOrg);

	OnOrganizationAdded.Broadcast();
	return NewId;
}

void UOrganizations::SetHeadquarters(const FOrganizationId InOrganizationId, const FBuildingId InBuildingId)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->SetHeadquarters(InOrganizationId, InBuildingId);
	OnHeadquartersChanged.Broadcast(InOrganizationId);
}

void UOrganizations::PublishAlignment(const FOrganizationId InOrganizationId, const TArray<float>& InAlignment)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->PublishAlignment(InOrganizationId, InAlignment);
	OnAlignmentPublished.Broadcast(InOrganizationId);
}

void UOrganizations::SettleNation(const FOrganizationId InOrganizationId, const FNationId InNationId)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	if (Entities->SettleNation(InOrganizationId, InNationId))
	{
		OnNationSettled.Broadcast(InOrganizationId);
	}
}

void UOrganizations::Dissolve(const FOrganizationId InOrganizationId)
{
	UOrganizationEntitySubsystem* Entities = GetEntities();
	if (Entities == nullptr)
	{
		return;
	}

	Entities->DestroyOrganization(InOrganizationId);
	OnOrganizationDissolved.Broadcast(InOrganizationId);
}
