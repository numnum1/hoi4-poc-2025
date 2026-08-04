#include "PlayerCharacterViewModel.h"

#include "ActionViewModel.h"
#include "BuildingKindViewModel.h"
#include "BuildingType.h"
#include "Buildings.h"
#include "Characters.h"
#include "FoundOrganizationModalWidgetViewModel.h"
#include "Kismet/GameplayStatics.h"
#include "PositionViewModel.h"
#include "Provinces.h"
#include "ResourceViewModel.h"
#include "RtsActions.h"
#include "RtsFacadeSubsystem.h"
#include "RtsPlayer.h"
#include "RtsResources.h"
#include "SelectablePositionViewModel.h"

void UPlayerCharacterViewModel::InitializeForPlayer()
{
	ARtsPlayer* NewPlayer = GetWorld() ? Cast<ARtsPlayer>(GetWorld()->GetFirstPlayerController()) : nullptr;
	if (Player == NewPlayer)
	{
		return;
	}

	if (Player)
	{
		Player->OnControlledCharacterIdChanged.RemoveAll(this);
	}

	Player = NewPlayer;

	if (Player)
	{
		Player->OnControlledCharacterIdChanged.AddDynamic(this, &UPlayerCharacterViewModel::HandleControlledCharacterChanged);
	}

	HandleControlledCharacterChanged();
}

void UPlayerCharacterViewModel::HandleControlledCharacterChanged()
{
	// Inherited from UCharacterViewModel — resolves the facades, rebinds the Characters events, and
	// calls Refresh(), which the override below drives the selection and the action list from.
	Initialize(Player ? Player->GetControlledCharacterId() : FCharacterId());
}

void UPlayerCharacterViewModel::Refresh_Implementation()
{
	Super::Refresh_Implementation();

	// Without a facade the inherited refresh bailed out and left its last values behind, so nothing
	// below may be trusted — the selection goes back to self and the modal closes, because a modal
	// belongs to the character that opened it rather than staying open over someone else.
	const bool bHasCharacters = Characters != nullptr;
	if (!bHasCharacters)
	{
		CloseModal();
	}

	// The picker is rebuilt first so the selection below can point at a current row.
	RefreshSelectablePositions();

	// A position the character no longer holds (or never held, on the first refresh) falls back to
	// acting as self rather than pointing at a stale index. Reapplying an unchanged selection also
	// re-gates the action list against the values that just changed.
	const bool bKeepSelection = bHasCharacters && GetPositions().IsValidIndex(SelectedPositionIndex);
	ApplySelection(bKeepSelection ? SelectedPositionIndex : INDEX_NONE);

	// An open modal has to survive a refresh with current values rather than with the ones it was
	// opened on: the character may have moved, been paid or gone broke since.
	RefreshModalValidation();
}

void UPlayerCharacterViewModel::SelectSelf()
{
	ApplySelection(INDEX_NONE);
}

void UPlayerCharacterViewModel::SelectPosition(const int32 InPositionIndex)
{
	if (!GetPositions().IsValidIndex(InPositionIndex))
	{
		return;
	}

	ApplySelection(InPositionIndex);
}

void UPlayerCharacterViewModel::ApplySelection(const int32 InPositionIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, InPositionIndex);
	UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, InPositionIndex == INDEX_NONE);

	USelectablePositionViewModel* NewSelectedPosition =
		SelectablePositions.IsValidIndex(InPositionIndex) ? SelectablePositions[InPositionIndex].Get() : nullptr;
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPosition, NewSelectedPosition);

	RefreshActions();
	RefreshResources();
}

void UPlayerCharacterViewModel::OpenModal(const EPlayerModal InModal)
{
	UE_MVVM_SET_PROPERTY_VALUE(ActiveModal, InModal);
	UE_MVVM_SET_PROPERTY_VALUE(bIsFoundOrganizationModalOpen, InModal == EPlayerModal::PM_FoundOrganization);
	UE_MVVM_SET_PROPERTY_VALUE(bIsViewProvinceModalOpen, InModal == EPlayerModal::PM_ViewProvince);

	// Whatever the last modal collected is not this one's starting point. The province is not reset
	// with it: that is where the player is looking on the map, not something this modal collected —
	// and starting the form on the province they were just inspecting is the useful default.
	UE_MVVM_SET_PROPERTY_VALUE(PendingOrganizationName, FText::GetEmpty());
	ApplyHeadquartersType(nullptr);
}

void UPlayerCharacterViewModel::CloseModal()
{
	OpenModal(EPlayerModal::PM_None);
}

void UPlayerCharacterViewModel::SetPendingOrganizationName(const FText& InName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PendingOrganizationName, InName);
	RefreshModalValidation();
}

void UPlayerCharacterViewModel::SelectProvince(const FProvinceId InProvinceId)
{
	UProvinces* ProvincesContainer = URtsFacadeSubsystem::Get(this) ? URtsFacadeSubsystem::Get(this)->GetProvincesContainer() : nullptr;

	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceId, InProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceIdValue, InProvinceId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName,
		ProvincesContainer && InProvinceId.IsValid() ? ProvincesContainer->GetDisplayInfo(InProvinceId).Name : FText::GetEmpty());

	// While the player is filling in the found-organization form, a map click is that form's site
	// field. Opening the province modal over it would throw the half-filled form away, so the
	// selection lands and nothing else moves.
	if (ActiveModal == EPlayerModal::PM_FoundOrganization)
	{
		RefreshModalValidation();
		return;
	}

	if (!InProvinceId.IsValid())
	{
		// The modal has nothing left to show, so it closes with the selection that fed it. Only that
		// one: another modal is not about the province and has no business being torn down here.
		if (ActiveModal == EPlayerModal::PM_ViewProvince)
		{
			CloseModal();
		}
		return;
	}

	OpenModal(EPlayerModal::PM_ViewProvince);

	USoundBase* InspectionSound = ProvincesContainer ? ProvincesContainer->GetDisplayInfo(InProvinceId).InspectionSound : nullptr;
	if (InspectionSound)
	{
		UGameplayStatics::PlaySound2D(this, InspectionSound);
	}
}

void UPlayerCharacterViewModel::ConfirmModal()
{
	if (!IsActionModal(ActiveModal))
	{
		return;
	}

	// Both gates run again here rather than trusting the bCanConfirmModal last pushed into the
	// widget — the same reason HandleActionClicked re-derives instead of believing bIsEnabled.
	const EAction ModalAction = GetModalAction();
	const FRtsActionContext Context = MakeContext();
	const FRtsActionInput Input = MakeInput();

	if (!URtsActions::GetBlockedReason(ModalAction, Context).IsEmpty()
		|| !URtsActions::GetInvalidInputReason(ModalAction, Context, Input).IsEmpty())
	{
		return;
	}

	URtsActions::ExecuteAction(ModalAction, Context, Input);
	CloseModal();
}

EAction UPlayerCharacterViewModel::GetModalAction() const
{
	switch (ActiveModal)
	{
	case EPlayerModal::PM_FoundOrganization:
		return EAction::EA_FoundOrganization;
	default:
		break;
	}
	return EAction::EA_FoundOrganization;
}

bool UPlayerCharacterViewModel::IsActionModal(const EPlayerModal InModal) const
{
	switch (InModal)
	{
	case EPlayerModal::PM_FoundOrganization:
		return true;
	default:
		return false;
	}
}

void UPlayerCharacterViewModel::SelectHeadquartersType(UBuildingType* InBuildingType)
{
	ApplyHeadquartersType(InBuildingType);
}

void UPlayerCharacterViewModel::ApplyHeadquartersType(UBuildingType* InBuildingType)
{
	UE_MVVM_SET_PROPERTY_VALUE(PendingHeadquartersType, InBuildingType);
	UE_MVVM_SET_PROPERTY_VALUE(PendingCost, InBuildingType ? InBuildingType->GetBuildCost() : 0);

	// Everything else about the pick — which row is highlighted, what the gate says — is derived
	// from the value just written, by the refresh below.
	RefreshModalValidation();
}

void UPlayerCharacterViewModel::HandleHeadquartersTypeClicked(UBuildingKindViewModel* ClickedType)
{
	if (ClickedType == nullptr)
	{
		return;
	}

	// Clicking the picked row unpicks it, exactly as clicking the selected position row does.
	UBuildingType* ClickedBuildingType = ClickedType->GetBuildingType();
	ApplyHeadquartersType(ClickedBuildingType == PendingHeadquartersType ? nullptr : ClickedBuildingType);
}

int32 UPlayerCharacterViewModel::GetCharacterFunds() const
{
	FRtsResourceContext Context;
	Context.Characters = Characters;
	Context.Organizations = Organizations;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		Context.Banking = Facades->GetBankingContainer();
	}

	// Deliberately the character rather than MakeResourceContext's owner: founding is paid for by
	// the player themselves, whatever they happen to be acting as at the moment.
	Context.Owner = FResourceOwner::ForCharacter(GetCharacterId());
	return URtsResources::GetAmount(EResourceType::RT_Money, EResourceDetail::RD_Total, Context);
}

void UPlayerCharacterViewModel::RefreshHeadquartersTypes()
{
	const int32 Funds = GetCharacterFunds();

	UBuildings* BuildingsContainer = URtsFacadeSubsystem::Get(this) ? URtsFacadeSubsystem::Get(this)->GetBuildingsContainer() : nullptr;
	const TArray<UBuildingType*> Catalogue = BuildingsContainer ? BuildingsContainer->GetHeadquarterBuildingTypes() : TArray<UBuildingType*>();

	TArray<TObjectPtr<UBuildingKindViewModel>> NewTypes;
	UBuildingKindViewModel* NewSelectedType = nullptr;

	for (UBuildingType* BuildingType : Catalogue)
	{
		TObjectPtr<UBuildingKindViewModel>& Entry = HeadquartersTypeCache.FindOrAdd(BuildingType);
		if (Entry == nullptr)
		{
			Entry = NewObject<UBuildingKindViewModel>(this);
			Entry->OnClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandleHeadquartersTypeClicked);
		}

		Entry->SetSource(BuildingType);
		Entry->SetAffordable(Funds >= (BuildingType ? BuildingType->GetBuildCost() : 0));

		const bool bIsPicked = BuildingType == PendingHeadquartersType;
		Entry->SetSelected(bIsPicked);
		if (bIsPicked)
		{
			NewSelectedType = Entry;
		}

		NewTypes.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypes, NewTypes);
	UE_MVVM_SET_PROPERTY_VALUE(HeadquartersTypeCount, NewTypes.Num());
	UE_MVVM_SET_PROPERTY_VALUE(SelectedHeadquartersType, NewSelectedType);
}

void UPlayerCharacterViewModel::RefreshModalValidation()
{
	// The rows carry prices and affordability, both of which move with the character's money — so
	// they are re-derived wherever the gate is, not only when the modal opens.
	RefreshHeadquartersTypes();

	// PM_ViewProvince only displays a UProvinceViewModel and has no EAction to gate — there is
	// nothing here for it to confirm.
	if (!IsActionModal(ActiveModal))
	{
		UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, FText::GetEmpty());
		UE_MVVM_SET_PROPERTY_VALUE(bCanConfirmModal, false);
		RefreshFoundOrganizationModal();
		return;
	}

	const EAction ModalAction = GetModalAction();
	const FRtsActionContext Context = MakeContext();

	// The action gate counts too: a modal opened a moment ago can become impossible while it is
	// still on screen, and confirming would then be refused with no visible reason.
	FText Reason = URtsActions::GetBlockedReason(ModalAction, Context);
	if (Reason.IsEmpty())
	{
		Reason = URtsActions::GetInvalidInputReason(ModalAction, Context, MakeInput());
	}

	UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, Reason);
	UE_MVVM_SET_PROPERTY_VALUE(bCanConfirmModal, Reason.IsEmpty());

	RefreshFoundOrganizationModal();
}

UFoundOrganizationModalWidgetViewModel* UPlayerCharacterViewModel::GetFoundOrganizationModalViewModel()
{
	if (FoundOrganizationModalViewModel == nullptr)
	{
		FoundOrganizationModalViewModel = NewObject<UFoundOrganizationModalWidgetViewModel>(this);
		FoundOrganizationModalViewModel->OnOrganizationNameChanged.AddDynamic(this, &UPlayerCharacterViewModel::HandleFoundOrganizationNameChanged);
		FoundOrganizationModalViewModel->OnConfirmClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandleFoundOrganizationConfirmClicked);
		FoundOrganizationModalViewModel->OnCancelClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandleFoundOrganizationCancelClicked);

		// Whoever asks for this before the first RefreshModalValidation ran (e.g. the root widget
		// pushing it into place in NativeOnInitialized) still gets a correctly-seeded instance.
		RefreshFoundOrganizationModal();
	}
	return FoundOrganizationModalViewModel;
}

void UPlayerCharacterViewModel::RefreshFoundOrganizationModal()
{
	GetFoundOrganizationModalViewModel()->SetSource(
		PendingOrganizationName,
		HeadquartersTypes,
		SelectedHeadquartersType,
		SelectedProvinceName,
		PendingCost,
		ModalErrorText,
		bCanConfirmModal);
}

void UPlayerCharacterViewModel::HandleFoundOrganizationNameChanged(const FText NewName)
{
	SetPendingOrganizationName(NewName);
}

void UPlayerCharacterViewModel::HandleFoundOrganizationConfirmClicked()
{
	ConfirmModal();
}

void UPlayerCharacterViewModel::HandleFoundOrganizationCancelClicked()
{
	CloseModal();
}

FRtsActionInput UPlayerCharacterViewModel::MakeInput() const
{
	FRtsActionInput Input;
	Input.OrganizationName = PendingOrganizationName;
	Input.HeadquartersType = PendingHeadquartersType;
	// The map selection *is* the site — there is no second "pending province" to keep in step with
	// what the player clicked.
	Input.HeadquartersProvinceId = SelectedProvinceId;
	return Input;
}

FRtsActionContext UPlayerCharacterViewModel::MakeContext() const
{
	FRtsActionContext Context;
	Context.ActorId = GetCharacterId();
	Context.Scope = bIsActingAsSelf ? ERtsActionScope::RAS_Self : ERtsActionScope::RAS_Position;
	Context.Characters = Characters;
	Context.Organizations = Organizations;
	Context.Roles = Roles;

	// Not inherited from UCharacterViewModel like the three above — resolved here, the same way
	// MakeResourceContext resolves Banking.
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		Context.Buildings = Facades->GetBuildingsContainer();
		Context.Banking = Facades->GetBankingContainer();
	}

	if (!bIsActingAsSelf)
	{
		// Organization and role are read from the same position index, so the context can never
		// claim a role at an organization the character does not hold it at.
		const UPositionViewModel* Position = GetPositions().IsValidIndex(SelectedPositionIndex) ? GetPositions()[SelectedPositionIndex].Get() : nullptr;
		Context.OrganizationId = Position ? Position->GetOrganizationId() : FOrganizationId();
		Context.RoleId = Position ? Position->GetRoleId() : FRoleId();
	}

	return Context;
}

FRtsResourceContext UPlayerCharacterViewModel::MakeResourceContext() const
{
	FRtsResourceContext Context;
	Context.Characters = Characters;
	Context.Organizations = Organizations;
	// Not inherited from UCharacterViewModel like Characters/Organizations are, so it is resolved
	// here directly — the same call URtsFacadeSubsystem::Get every other facade lookup goes through.
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		Context.Banking = Facades->GetBankingContainer();
	}

	if (bIsActingAsSelf)
	{
		Context.Owner = FResourceOwner::ForCharacter(GetCharacterId());
		return Context;
	}

	// Same lookup MakeContext uses for OrganizationId — the selected position's organization is
	// the resource owner while acting on its behalf.
	const UPositionViewModel* Position = GetPositions().IsValidIndex(SelectedPositionIndex) ? GetPositions()[SelectedPositionIndex].Get() : nullptr;
	if (Position && Position->GetOrganizationId().IsValid())
	{
		Context.Owner = FResourceOwner::ForOrganization(Position->GetOrganizationId());
	}

	return Context;
}

void UPlayerCharacterViewModel::RefreshSelectablePositions()
{
	// The rows are built from the UPositionViewModels the base class already refreshed, not from the
	// facade again — one read of the simulation, two shapes of it.
	const TArray<TObjectPtr<UPositionViewModel>>& CharacterPositions = GetPositions();

	TArray<TObjectPtr<USelectablePositionViewModel>> NewSelectablePositions;
	NewSelectablePositions.Reserve(CharacterPositions.Num());

	for (int32 Index = 0; Index < CharacterPositions.Num(); ++Index)
	{
		if (!SelectablePositionCache.IsValidIndex(Index))
		{
			USelectablePositionViewModel* NewEntry = NewObject<USelectablePositionViewModel>(this);
			NewEntry->OnClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandlePositionClicked);
			SelectablePositionCache.Add(NewEntry);
		}

		const UPositionViewModel* Position = CharacterPositions[Index];
		USelectablePositionViewModel* Entry = SelectablePositionCache[Index];
		Entry->SetSource(Index, Position->GetOrganizationName(), Position->GetOrganizationLogo());
		NewSelectablePositions.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(SelectablePositions, NewSelectablePositions);
	UE_MVVM_SET_PROPERTY_VALUE(SelectablePositionCount, NewSelectablePositions.Num());
}

void UPlayerCharacterViewModel::HandlePositionClicked(USelectablePositionViewModel* ClickedPosition)
{
	if (ClickedPosition == nullptr)
	{
		return;
	}

	// Clicking the selected row is how the player gets back out of a position: the selection clears,
	// every row goes unselected, and the character acts as themselves again.
	const int32 ClickedIndex = ClickedPosition->GetPositionIndex();
	if (ClickedIndex == SelectedPositionIndex)
	{
		SelectSelf();
		return;
	}

	SelectPosition(ClickedIndex);
}

void UPlayerCharacterViewModel::RefreshActions()
{
	const FRtsActionContext Context = MakeContext();

	TArray<EAction> OfferedActions;
	if (Context.Scope == ERtsActionScope::RAS_Self)
	{
		OfferedActions = URtsActions::GetActionsForSelf();
	}
	else
	{
		FPosition ActingPosition;
		ActingPosition.OrganizationId = Context.OrganizationId;
		ActingPosition.RoleId = Context.RoleId;
		OfferedActions = URtsActions::GetActionsForPosition(ActingPosition);
	}

	TArray<TObjectPtr<UActionViewModel>> NewActions;
	NewActions.Reserve(OfferedActions.Num());

	for (const EAction ActionType : OfferedActions)
	{
		TObjectPtr<UActionViewModel>& Entry = ActionEntryCache.FindOrAdd(ActionType);
		if (Entry == nullptr)
		{
			Entry = NewObject<UActionViewModel>(this);
			Entry->OnClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandleActionClicked);
		}

		Entry->SetAction(ActionType);

		const FText BlockedReason = URtsActions::GetBlockedReason(ActionType, Context);
		Entry->SetEnabled(BlockedReason.IsEmpty(), BlockedReason);

		NewActions.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Actions, NewActions);
	UE_MVVM_SET_PROPERTY_VALUE(ActionCount, NewActions.Num());
}

void UPlayerCharacterViewModel::RefreshResources()
{
	const FRtsResourceContext Context = MakeResourceContext();
	const TArray<EResourceType> OfferedResources = URtsResources::FindResourcesToDisplay(Context.Owner);

	TArray<TObjectPtr<UResourceViewModel>> NewResources;
	NewResources.Reserve(OfferedResources.Num());

	for (const EResourceType ResourceType : OfferedResources)
	{
		TObjectPtr<UResourceViewModel>& Entry = ResourceEntryCache.FindOrAdd(ResourceType);
		if (Entry == nullptr)
		{
			Entry = NewObject<UResourceViewModel>(this);
			Entry->OnClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandleResourceClicked);
		}

		Entry->SetSource(ResourceType, Context);
		Entry->SetExpanded(bHasExpandedResource && ResourceType == ExpandedResourceType);
		NewResources.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Resources, NewResources);
	UE_MVVM_SET_PROPERTY_VALUE(ResourceCount, NewResources.Num());
}

void UPlayerCharacterViewModel::HandleResourceClicked(UResourceViewModel* ClickedResource)
{
	if (ClickedResource == nullptr)
	{
		return;
	}

	const EResourceType ClickedType = ClickedResource->GetResourceType();
	const bool bWasExpanded = bHasExpandedResource && ExpandedResourceType == ClickedType;

	bHasExpandedResource = !bWasExpanded;
	ExpandedResourceType = ClickedType;

	// Re-derive every row's bIsExpanded from the new state rather than only touching the clicked
	// one — that is what actually enforces "only one expanded" against whichever row was open before.
	RefreshResources();
}

void UPlayerCharacterViewModel::HandleActionClicked(UActionViewModel* ClickedAction)
{
	if (ClickedAction == nullptr)
	{
		return;
	}

	// Re-derive the context and re-run the gate instead of trusting whichever bIsEnabled was last
	// pushed into the row — the same reason a React handler does not trust the rendered `disabled`.
	const FRtsActionContext Context = MakeContext();
	const EAction ActionType = ClickedAction->GetActionType();

	if (!URtsActions::GetBlockedReason(ActionType, Context).IsEmpty())
	{
		return;
	}

	// Actions that need input from the player open their modal instead of running; ConfirmModal
	// finishes the job. Which modal that is stays here rather than in URtsActions, because
	// EPlayerModal is UI state RtsViewModels is not allowed to know about.
	switch (ActionType)
	{
	case EAction::EA_FoundOrganization:
		OpenModal(EPlayerModal::PM_FoundOrganization);
		return;
	}
}

void UPlayerCharacterViewModel::BeginDestroy()
{
	if (Player)
	{
		Player->OnControlledCharacterIdChanged.RemoveAll(this);
	}
	Super::BeginDestroy();
}
