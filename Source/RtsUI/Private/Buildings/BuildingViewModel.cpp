#include "Buildings/BuildingViewModel.h"

#include "Buildings.h"
#include "RtsGameStateComponent.h"

void UBuildingViewModel::Initialize(const FBuildingId InBuildingId)
{
	UBuildings* NewBuildings = FindRtsGameStateComponent<UBuildings>(this);

	if (Buildings == NewBuildings && BuildingId == InBuildingId)
	{
		return;
	}

	Unbind();

	Buildings = NewBuildings;

	BuildingId = InBuildingId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(BuildingIdValue, BuildingId.Value);

	if (Buildings)
	{
		Buildings->OnBuildingDestroyed.AddDynamic(this, &UBuildingViewModel::HandleBuildingEvent);
		Buildings->OnBuildingOwnerChanged.AddDynamic(this, &UBuildingViewModel::HandleBuildingEvent);
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

void UBuildingViewModel::Refresh_Implementation()
{
	if (Buildings == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Buildings->IsAlive(BuildingId));

	UE_MVVM_SET_PROPERTY_VALUE(BuildingType, Buildings->GetBuildingType(BuildingId));

	const FProvinceId NewProvinceId = Buildings->GetProvince(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceId, NewProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, NewProvinceId.Value);

	const FCharacterId NewOwnerCharacterId = Buildings->GetOwnerCharacter(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterId, NewOwnerCharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterIdValue, NewOwnerCharacterId.Value);
}

void UBuildingViewModel::Unbind()
{
	if (Buildings)
	{
		Buildings->OnBuildingDestroyed.RemoveAll(this);
		Buildings->OnBuildingOwnerChanged.RemoveAll(this);
	}
}

void UBuildingViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
