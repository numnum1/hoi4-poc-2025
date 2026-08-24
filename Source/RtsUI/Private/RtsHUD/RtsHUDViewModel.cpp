#include "RtsHUD/RtsHUDViewModel.h"

#include "Actions/RtsActions.h"
#include "Banking.h"
#include "Buildings.h"
#include "Buildings/BuildingCategoryViewModel.h"
#include "Buildings/BuildingKindViewModel.h"
#include "Buildings/RtsBuildingCategories.h"
#include "BuildingType.h"
#include "Departments.h"
#include "MapModes/MapModeEntryViewModel.h"
#include "MapModes/RtsMapModes.h"
#include "Nations.h"
#include "Organizations.h"
#include "Populations.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"
#include "RtsMapPawn.h"
#include "RtsPlayer.h"

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
	Populations = FindRtsGameStateComponent<UPopulations>(this);
	Banking = FindRtsGameStateComponent<UBanking>(this);

	BindFacades();
	RefreshMapModeEntries();
	RefreshBuildMenu();
	BindPlayer();

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
}

void URtsHUDViewModel::BeginDestroy()
{
	UnbindFacades();

	if (Player)
	{
		Player->OnControlledCharacterIdChanged.RemoveAll(this);
	}

	if (Pawn)
	{
		Pawn->OnRightMouseButtonClicked.RemoveAll(this);
	}

	Super::BeginDestroy();
}

void URtsHUDViewModel::SetActiveModal(EPlayerModal InActiveModal)
{
	if (ActiveModal == InActiveModal)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(ActiveModal, InActiveModal);

	// The build menu belongs to whichever process needs a building picked, and today that is
	// founding an organization - so the modal opening brings the one menu up and closing it takes
	// it down again, pick and all.
	SetBuildMenuOpen(InActiveModal == EPlayerModal::PM_FoundOrganization);

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

void URtsHUDViewModel::SetSelectedProvinceId(const FProvinceId InSelectedProvinceId)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceId, InSelectedProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceIdValue, InSelectedProvinceId.Value);
}

void URtsHUDViewModel::SetHoveredProvinceId(const FProvinceId InHoveredProvinceId)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(HoveredProvinceId, InHoveredProvinceId))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeTooltip);
	}
}

TMap<FProvinceId, FProvinceVisuals> URtsHUDViewModel::GetProvinceVisuals() const
{
	FRtsMapModeContext Context;
	Context.Provinces = Provinces;
	Context.Nations = Nations;
	Context.Organizations = Organizations;
	Context.Departments = Departments;
	Context.Buildings = Buildings;
	Context.Populations = Populations;

	return URtsMapModes::BuildVisuals(ActiveMapMode, Context);
}

TMap<FNationId, FWorldMapNationData> URtsHUDViewModel::GetNationData() const
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

FMapTooltipContent URtsHUDViewModel::ComputeTooltip() const
{
	FMapTooltipContent Tooltip;

	if (Provinces == nullptr || !Provinces->IsAlive(HoveredProvinceId))
	{
		return Tooltip;
	}

	Tooltip.Headline = Provinces->GetDisplayInfo(HoveredProvinceId).Name;

	return Tooltip;
}

void URtsHUDViewModel::SetBuildMenuOpen(const bool bInOpen)
{
	if (bIsBuildMenuOpen == bInOpen)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsBuildMenuOpen, bInOpen);

	// Dropping the pick is what keeps a reopened menu from handing the next process a building the
	// player chose for the last one.
	ApplySelectedBuildingType(nullptr);
	RefreshBuildMenu();
}

void URtsHUDViewModel::ApplySelectedBuildingType(UBuildingType* InBuildingType)
{
	if (SelectedBuildingType == InBuildingType)
	{
		return;
	}

	SelectedBuildingType = InBuildingType;

	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingCost, InBuildingType ? InBuildingType->GetBuildCost() : 0);

	const TObjectPtr<UBuildingKindViewModel>* PickedRow = InBuildingType ? BuildingKindCache.Find(InBuildingType) : nullptr;
	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingKind, PickedRow ? PickedRow->Get() : nullptr);

	for (const TPair<TObjectPtr<UBuildingType>, TObjectPtr<UBuildingKindViewModel>>& Entry : BuildingKindCache)
	{
		if (Entry.Value)
		{
			Entry.Value->SetSelected(Entry.Key == InBuildingType);
		}
	}

	// What the tooltip says about a hovered province depends on what is about to be placed there.
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeTooltip);
}

void URtsHUDViewModel::RefreshBuildMenu()
{
	const bool bHeadquartersOnly = IsBuildMenuHeadquartersOnly();
	const int32 Funds = URtsActions::GetCharacterFunds(Banking, ControlledCharacterId);

	TArray<TObjectPtr<UBuildingCategoryViewModel>> NewCategories;

	for (const EBuildingCategory CategoryValue : URtsBuildingCategories::GetCategories())
	{
		const TArray<UBuildingType*> Catalogue = Buildings ? Buildings->GetBuildingTypesInCategory(CategoryValue) : TArray<UBuildingType*>();

		TArray<TObjectPtr<UBuildingKindViewModel>> Kinds;
		for (UBuildingType* BuildingType : Catalogue)
		{
			if (BuildingType == nullptr || (bHeadquartersOnly && !BuildingType->IsHeadquarters()))
			{
				continue;
			}

			TObjectPtr<UBuildingKindViewModel>& Kind = BuildingKindCache.FindOrAdd(BuildingType);
			if (Kind == nullptr)
			{
				Kind = NewObject<UBuildingKindViewModel>(this);
				Kind->OnClicked.AddDynamic(this, &URtsHUDViewModel::HandleBuildingKindClicked);
			}

			Kind->SetSource(BuildingType);
			Kind->SetAffordable(Funds >= BuildingType->GetBuildCost());
			Kind->SetSelected(BuildingType == SelectedBuildingType);

			Kinds.Add(Kind);
		}

		if (Kinds.IsEmpty())
		{
			continue;
		}

		TObjectPtr<UBuildingCategoryViewModel>& CategoryRow = BuildingCategoryCache.FindOrAdd(CategoryValue);
		if (CategoryRow == nullptr)
		{
			CategoryRow = NewObject<UBuildingCategoryViewModel>(this);
			CategoryRow->SetCategory(CategoryValue);
		}

		CategoryRow->SetKinds(Kinds);
		NewCategories.Add(CategoryRow);
	}

	UE_MVVM_SET_PROPERTY_VALUE(BuildingCategories, NewCategories);
	UE_MVVM_SET_PROPERTY_VALUE(BuildingCategoryCount, NewCategories.Num());

	// The filter can drop the picked kind out of the menu entirely, and a pick nobody can see any
	// more is the same stale state SetBuildMenuOpen clears.
	if (SelectedBuildingType && bHeadquartersOnly && !SelectedBuildingType->IsHeadquarters())
	{
		ApplySelectedBuildingType(nullptr);
	}
}

bool URtsHUDViewModel::IsBuildMenuHeadquartersOnly() const
{
	return ActiveModal == EPlayerModal::PM_FoundOrganization;
}

void URtsHUDViewModel::HandleBuildingKindClicked(UBuildingKindViewModel* InClickedKind)
{
	if (InClickedKind == nullptr)
	{
		return;
	}

	// Clicking the picked row unpicks it, the same toggle every other picker here uses.
	UBuildingType* ClickedType = InClickedKind->GetBuildingType();
	ApplySelectedBuildingType(ClickedType == SelectedBuildingType ? nullptr : ClickedType);
}

void URtsHUDViewModel::HandleBalanceChanged(FBankAccountId InAccountId)
{
	// Unfiltered: re-deriving the whole menu is cheap and affordability has to stay honest while it
	// is open.
	RefreshBuildMenu();
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
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
}

void URtsHUDViewModel::HandleProvinceChanged(FProvinceId InProvinceId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
	// Which provinces a nation holds is what the country labels are laid out over, so a change of
	// owner moves the labels, not just the colors.
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
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

void URtsHUDViewModel::HandlePopulationsSetup()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::HandlePopulationsGrown()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::HandlePopChanged(FPopId InPopId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
}

void URtsHUDViewModel::HandleNationDestroyed(FNationId InNationId)
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetNationData);
}

void URtsHUDViewModel::BindFacades()
{
	if (Provinces)
	{
		Provinces->OnProvincesSetup.AddDynamic(this, &URtsHUDViewModel::HandleProvincesSetup);
		Provinces->OnOwningNationChanged.AddDynamic(this, &URtsHUDViewModel::HandleProvinceChanged);
	}

	if (Nations)
	{
		Nations->OnNationDestroyed.AddDynamic(this, &URtsHUDViewModel::HandleNationDestroyed);
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

	// A pop shrinking below its neighbour flips the province's dominant ethnicity, so the ethnicity
	// mode has to follow size changes, not just creation.
	if (Populations)
	{
		Populations->OnPopulationsSetup.AddDynamic(this, &URtsHUDViewModel::HandlePopulationsSetup);
		Populations->OnPopulationsGrown.AddDynamic(this, &URtsHUDViewModel::HandlePopulationsGrown);
		Populations->OnPopSizeChanged.AddDynamic(this, &URtsHUDViewModel::HandlePopChanged);
		Populations->OnPopDestroyed.AddDynamic(this, &URtsHUDViewModel::HandlePopChanged);
	}

	if (Banking)
	{
		Banking->OnBalanceChanged.AddDynamic(this, &URtsHUDViewModel::HandleBalanceChanged);
	}
}

void URtsHUDViewModel::BindPlayer()
{
	ARtsPlayer* NewPlayer = GetWorld() ? Cast<ARtsPlayer>(GetWorld()->GetFirstPlayerController()) : nullptr;
	if (Player != NewPlayer)
	{
		if (Player)
		{
			Player->OnControlledCharacterIdChanged.RemoveAll(this);
		}

		Player = NewPlayer;

		if (Player)
		{
			Player->OnControlledCharacterIdChanged.AddDynamic(this, &URtsHUDViewModel::HandleControlledCharacterChanged);
		}
	}

	HandleControlledCharacterChanged();
}

void URtsHUDViewModel::HandleControlledCharacterChanged()
{
	if (UE_MVVM_SET_PROPERTY_VALUE(ControlledCharacterId, Player ? Player->GetControlledCharacterId() : FCharacterId()))
	{
		// Affordability is priced against whoever is being played.
		RefreshBuildMenu();
	}

	BindPawnRightClick();
}

void URtsHUDViewModel::BindPawnRightClick()
{
	ARtsMapPawn* NewPawn = Player ? Cast<ARtsMapPawn>(Player->GetPawn()) : nullptr;
	if (Pawn == NewPawn)
	{
		return;
	}

	if (Pawn)
	{
		Pawn->OnRightMouseButtonClicked.RemoveAll(this);
	}

	Pawn = NewPawn;

	if (Pawn)
	{
		Pawn->OnRightMouseButtonClicked.AddDynamic(this, &URtsHUDViewModel::HandleRightMouseButtonClicked);
	}
}

void URtsHUDViewModel::HandleRightMouseButtonClicked()
{
	SetSelectedProvinceId(FProvinceId());
}

void URtsHUDViewModel::UnbindFacades()
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

	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}
}
