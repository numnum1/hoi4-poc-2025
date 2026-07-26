#include "BuildingViewModel.h"

#include "BuildingProxy.h"
#include "CharacterProxy.h"
#include "OrganizationProxy.h"
#include "ProvinceProxy.h"
#include "RtsProxySubsystem.h"

void UBuildingViewModel::Initialize_Implementation(const FBuildingId InBuildingId)
{
	UBuildingProxy* NewProxy = nullptr;
	if (URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this))
	{
		NewProxy = ProxySubsystem->GetBuilding(InBuildingId);
	}

	if (Proxy == NewProxy && BuildingId == InBuildingId)
	{
		return;
	}

	Unbind();

	Proxy = NewProxy;

	BuildingId = InBuildingId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(BuildingIdValue, BuildingId.Value);

	if (Proxy)
	{
		Proxy->OnChanged.AddDynamic(this, &UBuildingViewModel::HandleProxyChanged);
	}

	Refresh();
}

void UBuildingViewModel::HandleProxyChanged()
{
	Refresh();
}

void UBuildingViewModel::Refresh()
{
	if (Proxy == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Proxy->IsAlive());

	UE_MVVM_SET_PROPERTY_VALUE(BuildingType, Proxy->GetBuildingType());

	const UProvinceProxy* Province = Proxy->GetProvince();
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceId, Province ? Province->GetId() : FProvinceId());
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, Province ? Province->GetId().Value : INDEX_NONE);

	const EBuildingOwnerType NewOwnerType = Proxy->GetOwnerType();
	UE_MVVM_SET_PROPERTY_VALUE(OwnerType, NewOwnerType);
	UE_MVVM_SET_PROPERTY_VALUE(bIsOwnedByCharacter, NewOwnerType == EBuildingOwnerType::BOT_Character);
	UE_MVVM_SET_PROPERTY_VALUE(bIsOwnedByOrganization, NewOwnerType == EBuildingOwnerType::BOT_Organization);

	const UCharacterProxy* OwnerCharacter = Proxy->GetOwnerCharacter();
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterId, OwnerCharacter ? OwnerCharacter->GetId() : FCharacterId());
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterIdValue, OwnerCharacter ? OwnerCharacter->GetId().Value : INDEX_NONE);

	const UOrganizationProxy* OwnerOrganization = Proxy->GetOwnerOrganization();
	UE_MVVM_SET_PROPERTY_VALUE(OwnerOrganizationId, OwnerOrganization ? OwnerOrganization->GetId() : FOrganizationId());
	UE_MVVM_SET_PROPERTY_VALUE(OwnerOrganizationIdValue, OwnerOrganization ? OwnerOrganization->GetId().Value : INDEX_NONE);
}

void UBuildingViewModel::Unbind()
{
	if (Proxy)
	{
		Proxy->OnChanged.RemoveAll(this);
	}
}

void UBuildingViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
