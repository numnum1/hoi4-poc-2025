#include "BuildingProxy.h"

#include "Buildings.h"
#include "CharacterProxy.h"
#include "OrganizationProxy.h"
#include "ProvinceProxy.h"
#include "RtsProxySubsystem.h"

void UBuildingProxy::Setup(URtsProxySubsystem* InProxies, const FBuildingId InBuildingId, UBuildings* InBuildings)
{
	Proxies = InProxies;
	BuildingId = InBuildingId;
	IdValue = InBuildingId.Value;
	Buildings = InBuildings;
	BindEvents();
}

void UBuildingProxy::BindEvents()
{
	if (Buildings == nullptr)
	{
		return;
	}

	Buildings->OnBuildingDestroyed.AddDynamic(this, &UBuildingProxy::HandleBuildingEvent);
	Buildings->OnBuildingOwnerChanged.AddDynamic(this, &UBuildingProxy::HandleBuildingEvent);
}

void UBuildingProxy::UnbindEvents()
{
	if (Buildings == nullptr)
	{
		return;
	}

	Buildings->OnBuildingDestroyed.RemoveAll(this);
	Buildings->OnBuildingOwnerChanged.RemoveAll(this);
}

void UBuildingProxy::HandleBuildingEvent(const FBuildingId InBuildingId)
{
	if (InBuildingId == BuildingId)
	{
		NotifyChanged();
	}
}

bool UBuildingProxy::IsAlive() const
{
	return Buildings != nullptr && Buildings->IsAlive(BuildingId);
}

UBuildingType* UBuildingProxy::GetBuildingType() const
{
	return Buildings ? Buildings->GetBuildingType(BuildingId) : nullptr;
}

UProvinceProxy* UBuildingProxy::GetProvince() const
{
	if (Buildings == nullptr || Proxies == nullptr)
	{
		return nullptr;
	}
	return Proxies->GetProvince(Buildings->GetProvince(BuildingId));
}

EBuildingOwnerType UBuildingProxy::GetOwnerType() const
{
	return Buildings ? Buildings->GetOwnerType(BuildingId) : EBuildingOwnerType::BOT_None;
}

UCharacterProxy* UBuildingProxy::GetOwnerCharacter() const
{
	if (Buildings == nullptr || Proxies == nullptr || GetOwnerType() != EBuildingOwnerType::BOT_Character)
	{
		return nullptr;
	}

	return Proxies->GetCharacter(Buildings->GetOwnerCharacter(BuildingId));
}

UOrganizationProxy* UBuildingProxy::GetOwnerOrganization() const
{
	if (Buildings == nullptr || Proxies == nullptr || GetOwnerType() != EBuildingOwnerType::BOT_Organization)
	{
		return nullptr;
	}

	return Proxies->GetOrganization(Buildings->GetOwnerOrganization(BuildingId));
}

void UBuildingProxy::DestroyBuilding()
{
	if (Buildings)
	{
		Buildings->Destroy(BuildingId);
	}
}
