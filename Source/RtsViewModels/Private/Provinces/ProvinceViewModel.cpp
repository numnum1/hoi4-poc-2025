#include "Resources/ProvinceViewModel.h"

#include "Buildings.h"
#include "Characters.h"
#include "Departments.h"
#include "NationInfo.h"
#include "Nations.h"
#include "OrganizationInfo.h"
#include "Organizations.h"
#include "Organizations/SelectableOrganizationViewModel.h"
#include "Presence/RtsPresence.h"
#include "Provinces.h"
#include "RtsFacadeSubsystem.h"

void UProvinceViewModel::Initialize(const FProvinceId InProvinceId)
{
	UProvinces* NewProvinces = nullptr;
	UNations* NewNations = nullptr;
	UOrganizations* NewOrganizations = nullptr;
	UDepartments* NewDepartments = nullptr;
	UBuildings* NewBuildings = nullptr;
	UCharacters* NewCharacters = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewProvinces = Facades->GetProvincesContainer();
		NewNations = Facades->GetNationsContainer();
		NewOrganizations = Facades->GetOrganizationsContainer();
		NewDepartments = Facades->GetDepartmentsContainer();
		NewBuildings = Facades->GetBuildingsContainer();
		NewCharacters = Facades->GetCharactersContainer();
	}

	if (Provinces == NewProvinces && ProvinceId == InProvinceId)
	{
		return;
	}

	Unbind();

	Provinces = NewProvinces;
	Nations = NewNations;
	Organizations = NewOrganizations;
	Departments = NewDepartments;
	Buildings = NewBuildings;
	Characters = NewCharacters;

	ProvinceId = InProvinceId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, ProvinceId.Value);

	if (Provinces)
	{
		Provinces->OnOwningNationChanged.AddDynamic(this, &UProvinceViewModel::HandleProvinceEvent);
		Provinces->OnProvinceDestroyed.AddDynamic(this, &UProvinceViewModel::HandleProvinceEvent);
	}

	if (Departments)
	{
		Departments->OnDepartmentCreated.AddDynamic(this, &UProvinceViewModel::HandlePresenceEvent);
		Departments->OnDepartmentDissolved.AddDynamic(this, &UProvinceViewModel::HandlePresenceEvent);
		Departments->OnDepartmentSeatChanged.AddDynamic(this, &UProvinceViewModel::HandlePresenceEvent);
	}

	if (Buildings)
	{
		Buildings->OnBuildingAdded.AddDynamic(this, &UProvinceViewModel::HandleBuildingEvent);
		Buildings->OnBuildingDestroyed.AddDynamic(this, &UProvinceViewModel::HandleBuildingEvent);
	}

	if (Characters)
	{
		Characters->OnCurrentProvinceChanged.AddDynamic(this, &UProvinceViewModel::HandleCharacterMoved);
	}

	Refresh();
}

void UProvinceViewModel::HandleProvinceEvent(const FProvinceId InProvinceId)
{
	if (InProvinceId == ProvinceId)
	{
		Refresh();
	}
}

void UProvinceViewModel::HandlePresenceEvent(const FDepartmentId InDepartmentId)
{
	Refresh();
}

void UProvinceViewModel::HandleBuildingEvent(const FBuildingId InBuildingId)
{
	Refresh();
}

void UProvinceViewModel::HandleCharacterMoved(const FCharacterId InCharacterId)
{
	Refresh();
}

void UProvinceViewModel::Refresh_Implementation()
{
	if (Provinces == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Provinces->IsAlive(ProvinceId));

	const FProvinceDisplayInfo DisplayInfo = Provinces->GetDisplayInfo(ProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(MapId, DisplayInfo.MapId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceName, DisplayInfo.Name);
	UE_MVVM_SET_PROPERTY_VALUE(Picture, DisplayInfo.Picture);
	UE_MVVM_SET_PROPERTY_VALUE(InspectionSound, DisplayInfo.InspectionSound);
	UE_MVVM_SET_PROPERTY_VALUE(Ecology, DisplayInfo.Ecology);

	const FNationId NewOwningNationId = Provinces->GetOwningNation(ProvinceId);
	const bool bHasOwner = Nations != nullptr && NewOwningNationId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(OwningNationId, NewOwningNationId);
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationIdValue, NewOwningNationId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationName, bHasOwner ? Nations->GetNationInfo(NewOwningNationId).NationName : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(OwningNationFlag, bHasOwner ? Nations->GetNationInfo(NewOwningNationId).Flag : FSlateNoResource());

	UE_MVVM_SET_PROPERTY_VALUE(TimezoneOffset, Provinces->GetTimezoneOffset(ProvinceId));

	RefreshOrganizationsPresent();
	UE_MVVM_SET_PROPERTY_VALUE(BuildingCount, Buildings ? Buildings->CollectBuildingsInProvince(ProvinceId).Num() : 0);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterCount, Characters ? Characters->CollectCharactersInProvince(ProvinceId).Num() : 0);
}

void UProvinceViewModel::RefreshOrganizationsPresent()
{
	const TArray<FOrganizationId> PresentIds = URtsPresence::CollectOrganizationsIn(ProvinceId, Departments, Buildings);

	TArray<TObjectPtr<USelectableOrganizationViewModel>> NewRows;
	NewRows.Reserve(PresentIds.Num());

	for (const FOrganizationId Id : PresentIds)
	{
		// No OnClicked binding here: this module cannot know what clicking a row should do (that is
		// UPlayerCharacterViewModel's business, in RtsUI). The row widget reports its click to
		// UProvinceDetailWidget::ViewOrganization instead.
		TObjectPtr<USelectableOrganizationViewModel>& Entry = OrganizationRowCache.FindOrAdd(Id);
		if (Entry == nullptr)
		{
			Entry = NewObject<USelectableOrganizationViewModel>(this);
		}

		const FOrganizationInfo Info = Organizations ? Organizations->GetOrganizationInfo(Id) : FOrganizationInfo();
		Entry->SetSource(Id, Info.OrganizationName, Info.Logo);
		NewRows.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(OrganizationsPresent, NewRows);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationsPresentCount, NewRows.Num());
}

void UProvinceViewModel::Unbind()
{
	if (Provinces)
	{
		Provinces->OnOwningNationChanged.RemoveAll(this);
		Provinces->OnProvinceDestroyed.RemoveAll(this);
	}

	if (Departments)
	{
		Departments->OnDepartmentCreated.RemoveAll(this);
		Departments->OnDepartmentDissolved.RemoveAll(this);
		Departments->OnDepartmentSeatChanged.RemoveAll(this);
	}

	if (Buildings)
	{
		Buildings->OnBuildingAdded.RemoveAll(this);
		Buildings->OnBuildingDestroyed.RemoveAll(this);
	}

	if (Characters)
	{
		Characters->OnCurrentProvinceChanged.RemoveAll(this);
	}
}

void UProvinceViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
