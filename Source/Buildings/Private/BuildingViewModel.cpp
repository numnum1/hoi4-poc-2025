#include "BuildingViewModel.h"

#include "Buildings.h"

void UBuildingViewModel::Initialize(UBuildings* InBuildings, const FBuildingId InBuildingId)
{
	if (BuildingsContainer == InBuildings && BuildingId == InBuildingId)
	{
		return;
	}

	Unbind();

	BuildingsContainer = InBuildings;

	BuildingId = InBuildingId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(BuildingIdValue, BuildingId.Value);

	if (BuildingsContainer)
	{
		BuildingsContainer->OnBuildingDestroyed.AddDynamic(this, &UBuildingViewModel::HandleBuildingEvent);
	}

	Refresh();
}

void UBuildingViewModel::HandleBuildingEvent(const FBuildingId InBuildingId)
{
	if (InBuildingId == BuildingId)
	{
		Refresh();
	}
}

void UBuildingViewModel::Refresh()
{
	if (BuildingsContainer == nullptr || !BuildingId.IsValid())
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, BuildingsContainer->IsAlive(BuildingId));

	UE_MVVM_SET_PROPERTY_VALUE(BuildingType, BuildingsContainer->GetBuildingType(BuildingId));

	const FProvinceId NewProvinceId = BuildingsContainer->GetProvince(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceId, NewProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, NewProvinceId.Value);

	const FBuildingOwner Owner = BuildingsContainer->GetBuildingOwner(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerType, Owner.OwnerType);
	UE_MVVM_SET_PROPERTY_VALUE(bIsOwnedByCharacter, Owner.OwnerType == EBuildingOwnerType::BOT_Character);
	UE_MVVM_SET_PROPERTY_VALUE(bIsOwnedByOrganization, Owner.OwnerType == EBuildingOwnerType::BOT_Organization);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterId, Owner.CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterIdValue, Owner.CharacterId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerOrganizationId, Owner.OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerOrganizationIdValue, Owner.OrganizationId.Value);
}

void UBuildingViewModel::Unbind()
{
	if (BuildingsContainer)
	{
		BuildingsContainer->OnBuildingDestroyed.RemoveAll(this);
	}
}

void UBuildingViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
