#include "BuildingViewModel.h"

#include "BuildingKinds.h"
#include "Buildings.h"
#include "RtsFacadeSubsystem.h"

void UBuildingViewModel::Initialize(const FBuildingId InBuildingId)
{
	UBuildings* NewBuildings = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewBuildings = Facades->GetBuildingsContainer();
	}

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

	const EBuildingKind NewKind = Buildings->GetKind(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(BuildingKind, NewKind);
	UE_MVVM_SET_PROPERTY_VALUE(BuildingKindName, UBuildingKinds::GetDisplayName(NewKind));
	UE_MVVM_SET_PROPERTY_VALUE(BuildingKindIcon, UBuildingKinds::GetIcon(NewKind));

	const FProvinceId NewProvinceId = Buildings->GetProvince(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceId, NewProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, NewProvinceId.Value);

	const EBuildingOwnerType NewOwnerType = Buildings->GetOwnerType(BuildingId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerType, NewOwnerType);
	UE_MVVM_SET_PROPERTY_VALUE(bIsOwnedByCharacter, NewOwnerType == EBuildingOwnerType::BOT_Character);
	UE_MVVM_SET_PROPERTY_VALUE(bIsOwnedByOrganization, NewOwnerType == EBuildingOwnerType::BOT_Organization);

	const FCharacterId NewOwnerCharacterId = NewOwnerType == EBuildingOwnerType::BOT_Character ? Buildings->GetOwnerCharacter(BuildingId) : FCharacterId();
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterId, NewOwnerCharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerCharacterIdValue, NewOwnerCharacterId.Value);

	const FOrganizationId NewOwnerOrganizationId = NewOwnerType == EBuildingOwnerType::BOT_Organization ? Buildings->GetOwnerOrganization(BuildingId) : FOrganizationId();
	UE_MVVM_SET_PROPERTY_VALUE(OwnerOrganizationId, NewOwnerOrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OwnerOrganizationIdValue, NewOwnerOrganizationId.Value);
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
