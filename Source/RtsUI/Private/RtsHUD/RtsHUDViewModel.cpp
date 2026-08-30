#include "RtsHUD/RtsHUDViewModel.h"

#include "Actions/RtsActions.h"
#include "Banking.h"
#include "BuildingCatalog.h"
#include "Buildings.h"
#include "BuildingType.h"
#include "Characters.h"
#include "Departments.h"
#include "MapModes/MapModeEntryViewModel.h"
#include "MapModes/RtsMapModes.h"
#include "Nations.h"
#include "Organizations.h"
#include "Populations.h"
#include "Provinces.h"
#include "Roles.h"
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
	Characters = FindRtsGameStateComponent<UCharacters>(this);
	Roles = FindRtsGameStateComponent<URoles>(this);

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
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeIsBuildMenuOpen);
	ApplySelectedBuildingType(nullptr);
	RefreshBuildMenu();

	// The typed name goes with the form it was typed into.
	if (InActiveModal != EPlayerModal::PM_FoundOrganization)
	{
		SetPendingOrganizationName(FText::GetEmpty());
	}

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
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName,
		Provinces && Provinces->IsAlive(InSelectedProvinceId) ? Provinces->GetDisplayInfo(InSelectedProvinceId).Name : FText::GetEmpty());

	BroadcastFoundOrganizationVerdict();
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
	// While a site is being picked, eligibility *is* what the map is for.
	if (ActiveModal == EPlayerModal::PM_FoundOrganization && SelectedBuildingType != nullptr)
	{
		return BuildFoundingSiteVisuals();
	}

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

bool URtsHUDViewModel::ComputeIsBuildMenuOpen() const
{
	return ActiveModal == EPlayerModal::PM_FoundOrganization;
}

void URtsHUDViewModel::ApplySelectedBuildingType(UBuildingType* InBuildingType)
{
	if (SelectedBuildingType == InBuildingType)
	{
		return;
	}

	SelectedBuildingType = InBuildingType;

	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingCost, InBuildingType ? InBuildingType->GetBuildCost() : 0);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedBuildingName, InBuildingType ? InBuildingType->GetBuildingName() : FText::GetEmpty());

	// What the tooltip says about a hovered province depends on what is about to be placed there,
	// and so does where the map says a site is free.
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeTooltip);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetProvinceVisuals);
	BroadcastFoundOrganizationVerdict();
}

void URtsHUDViewModel::SetPendingOrganizationName(const FText& InPendingOrganizationName)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(PendingOrganizationName, InPendingOrganizationName))
	{
		BroadcastFoundOrganizationVerdict();
	}
}

FText URtsHUDViewModel::ComputeFoundOrganizationError() const
{
	const FRtsActionContext Context = MakeFoundOrganizationContext();

	const FText BlockedReason = URtsActions::GetFoundOrganizationBlockedReason(Context);
	if (!BlockedReason.IsEmpty())
	{
		return BlockedReason;
	}

	return URtsActions::GetFoundOrganizationInvalidInputReason(Context, MakeFoundOrganizationInput());
}

bool URtsHUDViewModel::CanConfirmFoundOrganization() const
{
	return ComputeFoundOrganizationError().IsEmpty();
}

FRtsActionContext URtsHUDViewModel::MakeFoundOrganizationContext() const
{
	FRtsActionContext Context;
	Context.Scope = ERtsActionScope::RAS_Self;
	Context.ActorId = ControlledCharacterId;
	Context.Characters = Characters;
	Context.Organizations = Organizations;
	Context.Departments = Departments;
	Context.Roles = Roles;
	Context.Buildings = Buildings;
	Context.Banking = Banking;
	Context.Provinces = Provinces;
	return Context;
}

FRtsActionInput URtsHUDViewModel::MakeFoundOrganizationInput() const
{
	FRtsActionInput Input;
	Input.OrganizationName = PendingOrganizationName;
	Input.HeadquartersType = SelectedBuildingType;
	Input.HeadquartersProvinceId = SelectedProvinceId;
	return Input;
}

void URtsHUDViewModel::BroadcastFoundOrganizationVerdict()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeFoundOrganizationError);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CanConfirmFoundOrganization);
}

FNationId URtsHUDViewModel::GetActingNationId() const
{
	const FProvinceId ActorProvinceId = Characters ? Characters->GetCurrentProvince(ControlledCharacterId) : FProvinceId();
	return Provinces && ActorProvinceId.IsValid() ? Provinces->GetOwningNation(ActorProvinceId) : FNationId();
}

TMap<FProvinceId, FProvinceVisuals> URtsHUDViewModel::BuildFoundingSiteVisuals() const
{
	TMap<FProvinceId, FProvinceVisuals> Visuals;
	if (Provinces == nullptr)
	{
		return Visuals;
	}

	const FNationId ActingNationId = GetActingNationId();

	for (int32 Index = 0; Index < Provinces->Num(); ++Index)
	{
		const FProvinceId CandidateId(Index);
		if (!Provinces->IsAlive(CandidateId))
		{
			continue;
		}

		const FNationId OwningNation = Provinces->GetOwningNation(CandidateId);
		if (OwningNation.IsValid() && OwningNation != ActingNationId)
		{
			Visuals.Add(CandidateId).PrimaryColor = FColor(128, 128, 128);
			continue;
		}

		if (!URtsActions::CanFoundHeadquartersIn(CandidateId, Provinces, ActingNationId))
		{
			Visuals.Add(CandidateId).PrimaryColor = FColor::Red;
			continue;
		}

		bool bAlreadyHasType = false;
		if (Buildings)
		{
			for (const FBuildingId ExistingId : Buildings->CollectBuildingsInProvince(CandidateId))
			{
				if (Buildings->GetBuildingType(ExistingId) == SelectedBuildingType
					&& Buildings->GetOwnerCharacter(ExistingId) == ControlledCharacterId)
				{
					bAlreadyHasType = true;
					break;
				}
			}
		}

		Visuals.Add(CandidateId).PrimaryColor = bAlreadyHasType ? FColor::Blue : FColor::Green;
	}

	return Visuals;
}

void URtsHUDViewModel::RefreshBuildMenu()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeBuildings);

	// The filter can drop the picked kind out of the menu entirely, and a pick nobody can see any
	// more is the same stale state SetBuildMenuOpen clears.
	if (SelectedBuildingType && IsBuildMenuHeadquartersOnly() && !SelectedBuildingType->IsHeadquarters())
	{
		ApplySelectedBuildingType(nullptr);
	}
}

TArray<UBuildingType*> URtsHUDViewModel::ComputeBuildings() const
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::ReturnNull) : nullptr;
	const IBuildingCatalog* Catalog = World ? Cast<IBuildingCatalog>(World->GetAuthGameMode()) : nullptr;
	if (Catalog == nullptr)
	{
		return TArray<UBuildingType*>();
	}

	const bool bHeadquartersOnly = IsBuildMenuHeadquartersOnly();

	TArray<UBuildingType*> Result;
	for (const TObjectPtr<UBuildingType>& BuildingType : Catalog->GetBuildingTypes())
	{
		if (BuildingType == nullptr || (bHeadquartersOnly && !BuildingType->IsHeadquarters()))
		{
			continue;
		}

		Result.Add(BuildingType);
	}
	return Result;
}

bool URtsHUDViewModel::IsBuildMenuHeadquartersOnly() const
{
	return ActiveModal == EPlayerModal::PM_FoundOrganization;
}

void URtsHUDViewModel::HandleBalanceChanged(FBankAccountId InAccountId)
{
	// Unfiltered: re-deriving the whole menu is cheap and affordability has to stay honest while it
	// is open.
	RefreshBuildMenu();
	BroadcastFoundOrganizationVerdict();
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
		// Affordability is priced against whoever is being played, and so is everything founding
		// checks.
		RefreshBuildMenu();
		BroadcastFoundOrganizationVerdict();
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
