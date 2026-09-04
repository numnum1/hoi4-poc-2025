#include "MapModes/UseWorldMapView.h"

#include "Buildings.h"
#include "Characters.h"
#include "Departments.h"
#include "MapModes/MapModeEntryViewModel.h"
#include "MapModes/RtsMapModes.h"
#include "Nations.h"
#include "Organizations.h"
#include "Populations.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"

void UUseWorldMapView::Initialize()
{
	UnbindFacades();

	Provinces = FindRtsGameStateComponent<UProvinces>(this);
	Nations = FindRtsGameStateComponent<UNations>(this);
	Organizations = FindRtsGameStateComponent<UOrganizations>(this);
	Departments = FindRtsGameStateComponent<UDepartments>(this);
	Buildings = FindRtsGameStateComponent<UBuildings>(this);
	Populations = FindRtsGameStateComponent<UPopulations>(this);
	Characters = FindRtsGameStateComponent<UCharacters>(this);

	BindFacades();
	RefreshMapModeEntries();

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
}

void UUseWorldMapView::BeginDestroy()
{
	UnbindFacades();

	Super::BeginDestroy();
}

void UUseWorldMapView::SetActiveMapMode(const EMapMode InActiveMapMode)
{
	if (ActiveMapMode == InActiveMapMode)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(ActiveMapMode, InActiveMapMode);

	RefreshMapModeEntries();
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void UUseWorldMapView::SetHoveredProvinceId(const FProvinceId InHoveredProvinceId)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(HoveredProvinceId, InHoveredProvinceId))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeTooltip);
	}
}

void UUseWorldMapView::SetFoundingSiteOverlay(const FCharacterId InActorId, UBuildingType* InHeadquartersType)
{
	if (FoundingSiteActorId == InActorId && FoundingSiteHeadquartersType == InHeadquartersType)
	{
		return;
	}

	FoundingSiteActorId = InActorId;
	FoundingSiteHeadquartersType = InHeadquartersType;

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeTooltip);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

FRtsMapModeContext UUseWorldMapView::MakeContext() const
{
	FRtsMapModeContext Context;
	Context.Provinces = Provinces;
	Context.Nations = Nations;
	Context.Organizations = Organizations;
	Context.Departments = Departments;
	Context.Buildings = Buildings;
	Context.Populations = Populations;
	Context.Characters = Characters;

	return Context;
}

TMap<FProvinceId, FProvinceVisuals> UUseWorldMapView::GetProvinceVisuals() const
{
	// While a site is being picked, eligibility *is* what the map is for.
	if (FoundingSiteHeadquartersType != nullptr)
	{
		return URtsMapModes::BuildFoundingSiteVisuals(MakeContext(), FoundingSiteActorId, FoundingSiteHeadquartersType);
	}

	return URtsMapModes::BuildVisuals(ActiveMapMode, MakeContext());
}

TMap<FNationId, FWorldMapNationData> UUseWorldMapView::GetNationData() const
{
	TMap<FNationId, FWorldMapNationData> Result;
	if (Nations == nullptr || Provinces == nullptr)
	{
		return Result;
	}

	for (int32 Index = 0; Index < Nations->Num(); ++Index)
	{
		const FNationId NationId(Index);
		if (!Nations->IsAlive(NationId))
		{
			continue;
		}

		FWorldMapNationData Data;
		Data.Name = Nations->GetNationInfo(NationId).NationName;
		Data.OwnedProvinces = Provinces->CollectProvincesOfNation(NationId);

		// A nation without a single province has nothing on the map to label.
		if (!Data.OwnedProvinces.IsEmpty())
		{
			Result.Add(NationId, MoveTemp(Data));
		}
	}

	return Result;
}

FMapTooltipContent UUseWorldMapView::ComputeTooltip() const
{
	FMapTooltipContent Tooltip;

	if (Provinces == nullptr || !Provinces->IsAlive(HoveredProvinceId))
	{
		return Tooltip;
	}

	Tooltip.Headline = Provinces->GetDisplayInfo(HoveredProvinceId).Name;

	return Tooltip;
}

void UUseWorldMapView::RefreshMapModeEntries()
{
	const TArray<EMapMode> AvailableModes = URtsMapModes::GetMapModes();

	if (MapModes.Num() != AvailableModes.Num())
	{
		MapModes.Empty(AvailableModes.Num());

		for (const EMapMode MapMode : AvailableModes)
		{
			UMapModeEntryViewModel* Entry = NewObject<UMapModeEntryViewModel>(this);
			Entry->SetMapMode(MapMode);
			Entry->OnClicked.AddDynamic(this, &UUseWorldMapView::HandleMapModeClicked);
			MapModes.Add(Entry);
		}

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MapModes);
	}

	for (const TObjectPtr<UMapModeEntryViewModel>& Entry : MapModes)
	{
		Entry->SetActive(Entry->GetMapMode() == ActiveMapMode);
	}
}

void UUseWorldMapView::HandleMapModeClicked(UMapModeEntryViewModel* InClickedEntry)
{
	if (InClickedEntry)
	{
		SetActiveMapMode(InClickedEntry->GetMapMode());
	}
}

void UUseWorldMapView::HandleProvincesSetup()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
}

void UUseWorldMapView::HandleProvinceChanged(FProvinceId InProvinceId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
	// Which provinces a nation holds is what the country labels are laid out over, so a change of
	// owner moves the labels, not just the colors.
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
}

void UUseWorldMapView::HandleOrganizationsChanged()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void UUseWorldMapView::HandleOrganizationChanged(FOrganizationId InOrganizationId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void UUseWorldMapView::HandleDepartmentChanged(FDepartmentId InDepartmentId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void UUseWorldMapView::HandlePopulationsSetup()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void UUseWorldMapView::HandlePopulationsGrown()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void UUseWorldMapView::HandlePopChanged(FPopId InPopId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void UUseWorldMapView::HandleNationDestroyed(FNationId InNationId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
}

void UUseWorldMapView::BindFacades()
{
	if (Provinces)
	{
		Provinces->OnProvincesSetup.AddDynamic(this, &UUseWorldMapView::HandleProvincesSetup);
		Provinces->OnOwningNationChanged.AddDynamic(this, &UUseWorldMapView::HandleProvinceChanged);
	}

	if (Nations)
	{
		Nations->OnNationDestroyed.AddDynamic(this, &UUseWorldMapView::HandleNationDestroyed);
	}

	if (Organizations)
	{
		Organizations->OnOrganizationAdded.AddDynamic(this, &UUseWorldMapView::HandleOrganizationsChanged);
		Organizations->OnNationSettled.AddDynamic(this, &UUseWorldMapView::HandleOrganizationChanged);
		Organizations->OnOrganizationDissolved.AddDynamic(this, &UUseWorldMapView::HandleOrganizationChanged);
		Organizations->OnRootDepartmentChanged.AddDynamic(this, &UUseWorldMapView::HandleOrganizationChanged);
	}

	// A headquarters province comes from the root department's seat, so a reseated department moves
	// the map even though no organization changed.
	if (Departments)
	{
		Departments->OnDepartmentSeatChanged.AddDynamic(this, &UUseWorldMapView::HandleDepartmentChanged);
	}

	// A pop shrinking below its neighbour flips the province's dominant ethnicity, so the ethnicity
	// mode has to follow size changes, not just creation.
	if (Populations)
	{
		Populations->OnPopulationsSetup.AddDynamic(this, &UUseWorldMapView::HandlePopulationsSetup);
		Populations->OnPopulationsGrown.AddDynamic(this, &UUseWorldMapView::HandlePopulationsGrown);
		Populations->OnPopSizeChanged.AddDynamic(this, &UUseWorldMapView::HandlePopChanged);
		Populations->OnPopDestroyed.AddDynamic(this, &UUseWorldMapView::HandlePopChanged);
	}
}

void UUseWorldMapView::UnbindFacades()
{
	if (Provinces)
	{
		Provinces->OnProvincesSetup.RemoveAll(this);
		Provinces->OnOwningNationChanged.RemoveAll(this);
	}

	if (Nations)
	{
		Nations->OnNationDestroyed.RemoveAll(this);
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

	if (Populations)
	{
		Populations->OnPopulationsSetup.RemoveAll(this);
		Populations->OnPopulationsGrown.RemoveAll(this);
		Populations->OnPopSizeChanged.RemoveAll(this);
		Populations->OnPopDestroyed.RemoveAll(this);
	}
}
