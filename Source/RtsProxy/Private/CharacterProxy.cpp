#include "CharacterProxy.h"

#include "BuildingProxy.h"
#include "Buildings.h"
#include "Characters.h"
#include "NationProxy.h"
#include "OrganizationProxy.h"
#include "ProvinceProxy.h"
#include "RtsProxySubsystem.h"
#include "ScheduleProxy.h"

void UCharacterProxy::Setup(URtsProxySubsystem* InProxies, const FCharacterId InCharacterId, UCharacters* InCharacters)
{
	Proxies = InProxies;
	CharacterId = InCharacterId;
	IdValue = InCharacterId.Value;
	Characters = InCharacters;
	BindEvents();
}

void UCharacterProxy::BindEvents()
{
	if (Characters == nullptr)
	{
		return;
	}

	// Every event that can change something reachable from here, collapsed into OnChanged.
	Characters->OnCurrentProvinceChanged.AddDynamic(this, &UCharacterProxy::HandleCharacterEvent);
	Characters->OnScheduleChanged.AddDynamic(this, &UCharacterProxy::HandleCharacterEvent);
	Characters->OnPositionChanged.AddDynamic(this, &UCharacterProxy::HandleCharacterEvent);
	Characters->OnSimulated.AddDynamic(this, &UCharacterProxy::HandleCharacterEvent);
	Characters->OnCharacterDestroyed.AddDynamic(this, &UCharacterProxy::HandleCharacterEvent);
}

void UCharacterProxy::UnbindEvents()
{
	if (Characters == nullptr)
	{
		return;
	}

	Characters->OnCurrentProvinceChanged.RemoveAll(this);
	Characters->OnScheduleChanged.RemoveAll(this);
	Characters->OnPositionChanged.RemoveAll(this);
	Characters->OnSimulated.RemoveAll(this);
	Characters->OnCharacterDestroyed.RemoveAll(this);
}

void UCharacterProxy::HandleCharacterEvent(const FCharacterId InCharacterId)
{
	if (InCharacterId == CharacterId)
	{
		NotifyChanged();
	}
}

bool UCharacterProxy::IsAlive() const
{
	return Characters != nullptr && Characters->IsAlive(CharacterId);
}

FText UCharacterProxy::GetDisplayName() const
{
	return Characters ? Characters->GetDisplayInfo(CharacterId).CharacterName : FText::GetEmpty();
}

FSlateBrush UCharacterProxy::GetPortrait() const
{
	return Characters ? Characters->GetDisplayInfo(CharacterId).Portrait : FSlateBrush();
}

FAbilities UCharacterProxy::GetAbilities() const
{
	return Characters ? Characters->GetAbilities(CharacterId) : FAbilities();
}

FCharacterVitals UCharacterProxy::GetVitals() const
{
	return Characters ? Characters->GetVitals(CharacterId) : FCharacterVitals();
}

FCharacterSkills UCharacterProxy::GetSkills() const
{
	return Characters ? Characters->GetSkills(CharacterId) : FCharacterSkills();
}

FSchedule UCharacterProxy::GetSchedule() const
{
	return Characters ? Characters->GetSchedule(CharacterId) : FSchedule();
}

EScheduleEntryType UCharacterProxy::GetScheduleAtHour(const int32 InHour) const
{
	if (InHour < 0 || InHour > 23)
	{
		return EScheduleEntryType::SET_NONE;
	}
	return GetSchedule().GetAtHour(static_cast<uint8>(InHour));
}

UScheduleProxy* UCharacterProxy::GetScheduleTemplate() const
{
	if (Characters == nullptr || Proxies == nullptr)
	{
		return nullptr;
	}
	return Proxies->GetSchedule(Characters->GetScheduleId(CharacterId));
}

UProvinceProxy* UCharacterProxy::GetProvince() const
{
	if (Characters == nullptr || Proxies == nullptr)
	{
		return nullptr;
	}
	return Proxies->GetProvince(Characters->GetCurrentProvince(CharacterId));
}

UNationProxy* UCharacterProxy::GetNation() const
{
	const UProvinceProxy* Province = GetProvince();
	return Province ? Province->GetOwner() : nullptr;
}

TArray<UOrganizationProxy*> UCharacterProxy::GetOrganizations() const
{
	TArray<UOrganizationProxy*> Result;
	if (Characters == nullptr || Proxies == nullptr)
	{
		return Result;
	}

	const TArray<FPosition> Positions = Characters->GetPositions(CharacterId);
	Result.Reserve(Positions.Num());
	for (const FPosition& Position : Positions)
	{
		if (UOrganizationProxy* Organization = Proxies->GetOrganization(Position.OrganizationId))
		{
			Result.Add(Organization);
		}
	}
	return Result;
}

UOrganizationProxy* UCharacterProxy::GetOrganization(const int32 InIndex) const
{
	if (Characters == nullptr || Proxies == nullptr)
	{
		return nullptr;
	}

	const TArray<FPosition> Positions = Characters->GetPositions(CharacterId);
	return Positions.IsValidIndex(InIndex) ? Proxies->GetOrganization(Positions[InIndex].OrganizationId) : nullptr;
}

int32 UCharacterProxy::GetOrganizationCount() const
{
	return Characters ? Characters->GetPositions(CharacterId).Num() : 0;
}

bool UCharacterProxy::IsMemberOf(const UOrganizationProxy* InOrganization) const
{
	if (Characters == nullptr || InOrganization == nullptr)
	{
		return false;
	}

	const FOrganizationId Wanted = InOrganization->GetId();
	for (const FPosition& Position : Characters->GetPositions(CharacterId))
	{
		if (Position.OrganizationId == Wanted)
		{
			return true;
		}
	}
	return false;
}

TArray<UBuildingProxy*> UCharacterProxy::GetOwnedBuildings() const
{
	TArray<UBuildingProxy*> Result;
	if (Proxies == nullptr)
	{
		return Result;
	}

	// UBuildings has no owner index, so this walks the whole list. That is the deal proxies make:
	// convenience for the player-facing code, never a path the simulation takes.
	UBuildings* BuildingsContainer = Proxies->GetBuildingsContainer();
	if (BuildingsContainer == nullptr)
	{
		return Result;
	}

	for (int32 Index = 0; Index < BuildingsContainer->Num(); ++Index)
	{
		const FBuildingId BuildingId(Index);
		if (!BuildingsContainer->IsAlive(BuildingId))
		{
			continue;
		}

		const FBuildingOwner Owner = BuildingsContainer->GetBuildingOwner(BuildingId);
		if (Owner.OwnerType != EBuildingOwnerType::BOT_Character || Owner.CharacterId != CharacterId)
		{
			continue;
		}

		if (UBuildingProxy* Building = Proxies->GetBuilding(BuildingId))
		{
			Result.Add(Building);
		}
	}
	return Result;
}

void UCharacterProxy::MoveTo(const UProvinceProxy* InProvince)
{
	if (Characters == nullptr || InProvince == nullptr)
	{
		return;
	}
	Characters->MoveCharacter(CharacterId, InProvince->GetId());
}

void UCharacterProxy::JoinOrganization(const UOrganizationProxy* InOrganization)
{
	if (Characters == nullptr || InOrganization == nullptr)
	{
		return;
	}
	Characters->AddPosition(CharacterId, InOrganization->GetId());
}

void UCharacterProxy::DestroyCharacter()
{
	if (Characters)
	{
		Characters->Destroy(CharacterId);
	}
}
