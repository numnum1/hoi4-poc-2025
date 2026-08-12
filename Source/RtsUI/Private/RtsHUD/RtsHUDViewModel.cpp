#include "RtsHUD/RtsHUDViewModel.h"

#include "Buildings.h"
#include "Departments.h"
#include "MapModes/MapModeEntryViewModel.h"
#include "MapModes/RtsMapModes.h"
#include "Nations.h"
#include "Organizations.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"

URtsHUDViewModel::URtsHUDViewModel()
{

}

void URtsHUDViewModel::Initialize()
{
	UnbindFacades();

	Provinces = FindRtsGameStateComponent<UProvinces>(this);
	Nations = FindRtsGameStateComponent<UNations>(this);
	Organizations = FindRtsGameStateComponent<UOrganizations>(this);
	Departments = FindRtsGameStateComponent<UDepartments>(this);
	Buildings = FindRtsGameStateComponent<UBuildings>(this);

	BindFacades();
	RefreshMapModeEntries();

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::BeginDestroy()
{
	UnbindFacades();

	Super::BeginDestroy();
}

void URtsHUDViewModel::SetControlledCharacterId(FCharacterId InControlledCharacterId)
{
	UE_MVVM_SET_PROPERTY_VALUE(ControlledCharacterId, InControlledCharacterId);
}

void URtsHUDViewModel::SetActiveModal(EPlayerModal InActiveModal)
{
	if (ActiveModal == InActiveModal)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(ActiveModal, InActiveModal);

	// A modal may paint the map for its own purposes while it is open (the founding dialog colors
	// provinces by eligibility). Repainting the active mode on every modal change is what puts the
	// map back afterwards, so no modal has to undo its own colors.
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::SetSelectedPositionIndex(int32 InSelectedPositionIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, InSelectedPositionIndex);
}

void URtsHUDViewModel::SetActiveMapMode(const EMapMode InActiveMapMode)
{
	if (ActiveMapMode == InActiveMapMode)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(ActiveMapMode, InActiveMapMode);

	RefreshMapModeEntries();
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

TMap<FProvinceId, FProvinceVisuals> URtsHUDViewModel::GetProvinceVisuals() const
{
	FRtsMapModeContext Context;
	Context.Provinces = Provinces;
	Context.Nations = Nations;
	Context.Organizations = Organizations;
	Context.Departments = Departments;
	Context.Buildings = Buildings;

	return URtsMapModes::BuildVisuals(ActiveMapMode, Context);
}

void URtsHUDViewModel::RefreshMapModeEntries()
{
	const TArray<EMapMode> AvailableModes = URtsMapModes::GetMapModes();

	if (MapModes.Num() != AvailableModes.Num())
	{
		MapModes.Empty(AvailableModes.Num());

		for (const EMapMode MapMode : AvailableModes)
		{
			UMapModeEntryViewModel* Entry = NewObject<UMapModeEntryViewModel>(this);
			Entry->SetMapMode(MapMode);
			Entry->OnClicked.AddDynamic(this, &URtsHUDViewModel::HandleMapModeClicked);
			MapModes.Add(Entry);
		}

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MapModes);
	}

	for (const TObjectPtr<UMapModeEntryViewModel>& Entry : MapModes)
	{
		Entry->SetActive(Entry->GetMapMode() == ActiveMapMode);
	}
}

void URtsHUDViewModel::HandleMapModeClicked(UMapModeEntryViewModel* InClickedEntry)
{
	if (InClickedEntry)
	{
		SetActiveMapMode(InClickedEntry->GetMapMode());
	}
}

void URtsHUDViewModel::HandleProvincesSetup()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::HandleProvinceChanged(FProvinceId InProvinceId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::HandleOrganizationsChanged()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::HandleOrganizationChanged(FOrganizationId InOrganizationId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::HandleDepartmentChanged(FDepartmentId InDepartmentId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::BindFacades()
{
	if (Provinces)
	{
		Provinces->OnProvincesSetup.AddDynamic(this, &URtsHUDViewModel::HandleProvincesSetup);
		Provinces->OnOwningNationChanged.AddDynamic(this, &URtsHUDViewModel::HandleProvinceChanged);
	}

	if (Organizations)
	{
		Organizations->OnOrganizationAdded.AddDynamic(this, &URtsHUDViewModel::HandleOrganizationsChanged);
		Organizations->OnNationSettled.AddDynamic(this, &URtsHUDViewModel::HandleOrganizationChanged);
		Organizations->OnOrganizationDissolved.AddDynamic(this, &URtsHUDViewModel::HandleOrganizationChanged);
		Organizations->OnRootDepartmentChanged.AddDynamic(this, &URtsHUDViewModel::HandleOrganizationChanged);
	}

	// A headquarters province comes from the root department's seat, so a reseated department moves
	// the map even though no organization changed.
	if (Departments)
	{
		Departments->OnDepartmentSeatChanged.AddDynamic(this, &URtsHUDViewModel::HandleDepartmentChanged);
	}
}

void URtsHUDViewModel::UnbindFacades()
{
	if (Provinces)
	{
		Provinces->OnProvincesSetup.RemoveAll(this);
		Provinces->OnOwningNationChanged.RemoveAll(this);
	}

	if (Organizations)
	{
		Organizations->OnOrganizationAdded.RemoveAll(this);
		Organizations->OnNationSettled.RemoveAll(this);
		Organizations->OnOrganizationDissolved.RemoveAll(this);
		Organizations->OnRootDepartmentChanged.RemoveAll(this);
	}

	if (Departments)
	{
		Departments->OnDepartmentSeatChanged.RemoveAll(this);
	}
}
