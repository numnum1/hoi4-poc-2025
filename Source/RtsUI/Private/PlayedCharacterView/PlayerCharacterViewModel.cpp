#include "PlayedCharacterView/PlayerCharacterViewModel.h"

#include "Actions/RtsActions.h"
#include "Banking.h"
#include "Buildings.h"
#include "BuildingType.h"
#include "CharacterInfo.h"
#include "Characters.h"
#include "Characters/PositionViewModel.h"
#include "Characters/SelectableCharacterViewModel.h"
#include "Departments.h"
#include "Kismet/GameplayStatics.h"
#include "OrganizationInfo.h"
#include "Organizations.h"
#include "Organizations/SelectableOrganizationViewModel.h"
#include "Organizations/SelectablePositionViewModel.h"
#include "Presence/RtsPresence.h"
#include "Provinces.h"
#include "Resources/ResourceViewModel.h"
#include "Resources/RtsResources.h"
#include "RtsGameStateComponent.h"
#include "RtsPlayer.h"

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
	// Captured before Super rebuilds Positions: which organization (not which index) was selected.
	// A position lower in the list can now disappear — EA_LeaveOrganization removes one — which
	// shifts every later index down. Re-deriving the selection by index alone would silently land
	// on a different organization's position; identity is what actually has to survive a refresh.
	const bool bWasActingAsPosition = !bIsActingAsSelf && GetPositions().IsValidIndex(SelectedPositionIndex);
	const FOrganizationId PreviouslySelectedOrganizationId =
		bWasActingAsPosition ? GetPositions()[SelectedPositionIndex]->GetOrganizationId() : FOrganizationId();

	BindBanking();

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
	int32 NewSelectedPositionIndex = INDEX_NONE;
	if (bHasCharacters && PreviouslySelectedOrganizationId.IsValid())
	{
		const TArray<TObjectPtr<UPositionViewModel>>& CurrentPositions = GetPositions();
		for (int32 Index = 0; Index < CurrentPositions.Num(); ++Index)
		{
			if (CurrentPositions[Index] && CurrentPositions[Index]->GetOrganizationId() == PreviouslySelectedOrganizationId)
			{
				NewSelectedPositionIndex = Index;
				break;
			}
		}
	}
	ApplySelection(NewSelectedPositionIndex);

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
	UE_MVVM_SET_PROPERTY_VALUE(bIsJoinOrganizationModalOpen, InModal == EPlayerModal::PM_JoinOrganization);
	UE_MVVM_SET_PROPERTY_VALUE(bIsViewProvinceModalOpen, InModal == EPlayerModal::PM_ViewProvince);
	UE_MVVM_SET_PROPERTY_VALUE(bIsViewOrganizationModalOpen, InModal == EPlayerModal::PM_ViewOrganization);
	UE_MVVM_SET_PROPERTY_VALUE(bIsRecruitCharacterModalOpen, InModal == EPlayerModal::PM_RecruitCharacter);

	// Whatever the last modal collected is not this one's starting point. The province is not reset
	// with it: that is where the player is looking on the map, not something this modal collected —
	// and starting the form on the province they were just inspecting is the useful default. The
	// found-organization form resets itself (see UFoundOrganizationModalWidgetViewModel::ResetForm),
	// there being no state of its own left here to clear.
	ApplyJoinTarget(FOrganizationId());
	ApplyRecruitTarget(FCharacterId());
}

void UPlayerCharacterViewModel::CloseModal()
{
	OpenModal(EPlayerModal::PM_None);
}

void UPlayerCharacterViewModel::SelectProvince(const FProvinceId InProvinceId)
{
	UProvinces* ProvincesContainer = FindRtsGameStateComponent<UProvinces>(this);

	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceId, InProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceIdValue, InProvinceId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName,
		ProvincesContainer && InProvinceId.IsValid() ? ProvincesContainer->GetDisplayInfo(InProvinceId).Name : FText::GetEmpty());

	// While the player is filling in the found-organization form, a map click is that form's site
	// field. Opening the province modal over it would throw the half-filled form away, so the
	// selection lands and nothing else moves — the form reads the site off URtsHUDViewModel, which
	// the same click updates.
	if (ActiveModal == EPlayerModal::PM_FoundOrganization)
	{
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

void UPlayerCharacterViewModel::ViewOrganization(const FOrganizationId InOrganizationId)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedOrganizationId, InOrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedOrganizationIdValue, InOrganizationId.Value);
	OpenModal(EPlayerModal::PM_ViewOrganization);
}

void UPlayerCharacterViewModel::ConfirmModal()
{
	if (!IsActionModal(ActiveModal))
	{
		return;
	}

	// Both gates run again here rather than trusting the bCanConfirmModal last pushed into the
	// widget — the same reason every ClickXxx re-derives instead of believing bIsXxxEnabled.
	if (!ComputeModalError().IsEmpty())
	{
		return;
	}

	const FRtsActionContext Context = MakeContext();
	const FRtsActionInput Input = MakeInput();

	switch (ActiveModal)
	{
	case EPlayerModal::PM_JoinOrganization:
		URtsActions::ExecuteJoinOrganization(Context, Input);
		break;

	case EPlayerModal::PM_RecruitCharacter:
		URtsActions::ExecuteRecruitCharacter(Context, Input);
		break;

	default:
		return;
	}

	CloseModal();
}

void UPlayerCharacterViewModel::HandleFoundOrganizationConfirmed(const FText& InOrganizationName, UBuildingType* InHeadquartersType, const FProvinceId InProvinceId)
{
	// Re-run both gates on exactly what the form reported rather than trusting its own bIsValid —
	// the same rule ConfirmModal follows for the other two modals.
	const FRtsActionContext Context = MakeContext();

	FRtsActionInput Input;
	Input.OrganizationName = InOrganizationName;
	Input.HeadquartersType = InHeadquartersType;
	Input.HeadquartersProvinceId = InProvinceId;

	if (!URtsActions::GetFoundOrganizationBlockedReason(Context).IsEmpty()
		|| !URtsActions::GetFoundOrganizationInvalidInputReason(Context, Input).IsEmpty())
	{
		return;
	}

	URtsActions::ExecuteFoundOrganization(Context, Input);
	CloseModal();
}

FText UPlayerCharacterViewModel::ComputeModalError() const
{
	const FRtsActionContext Context = MakeContext();

	// The action gate counts too, not just the input: a modal opened a moment ago can become
	// impossible while it is still on screen, and confirming would then be refused with no visible
	// reason.
	switch (ActiveModal)
	{
	case EPlayerModal::PM_JoinOrganization:
	{
		const FText BlockedReason = URtsActions::GetJoinOrganizationBlockedReason(Context);
		return BlockedReason.IsEmpty() ? URtsActions::GetJoinOrganizationInvalidInputReason(Context, MakeInput()) : BlockedReason;
	}

	case EPlayerModal::PM_RecruitCharacter:
	{
		const FText BlockedReason = URtsActions::GetRecruitCharacterBlockedReason(Context);
		return BlockedReason.IsEmpty() ? URtsActions::GetRecruitCharacterInvalidInputReason(Context, MakeInput()) : BlockedReason;
	}

	default:
		break;
	}

	return FText::GetEmpty();
}

bool UPlayerCharacterViewModel::IsActionModal(const EPlayerModal InModal) const
{
	switch (InModal)
	{
	case EPlayerModal::PM_JoinOrganization:
	case EPlayerModal::PM_RecruitCharacter:
		return true;
	default:
		return false;
	}
}

void UPlayerCharacterViewModel::SelectJoinTarget(const FOrganizationId InOrganizationId)
{
	ApplyJoinTarget(InOrganizationId);
}

void UPlayerCharacterViewModel::ApplyJoinTarget(const FOrganizationId InOrganizationId)
{
	UE_MVVM_SET_PROPERTY_VALUE(PendingJoinOrganizationId, InOrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(PendingJoinOrganizationIdValue, InOrganizationId.Value);

	// Everything else about the pick — which row is highlighted, what the gate says — is derived
	// from the value just written, by the refresh below.
	RefreshModalValidation();
}

void UPlayerCharacterViewModel::HandleJoinableOrganizationClicked(USelectableOrganizationViewModel* ClickedOrganization)
{
	if (ClickedOrganization == nullptr)
	{
		return;
	}

	// Clicking the picked row unpicks it, exactly as clicking the selected position row does.
	const FOrganizationId ClickedId = ClickedOrganization->GetOrganizationId();
	ApplyJoinTarget(ClickedId == PendingJoinOrganizationId ? FOrganizationId() : ClickedId);
}

void UPlayerCharacterViewModel::SelectRecruitTarget(const FCharacterId InCharacterId)
{
	ApplyRecruitTarget(InCharacterId);
}

void UPlayerCharacterViewModel::ApplyRecruitTarget(const FCharacterId InCharacterId)
{
	UE_MVVM_SET_PROPERTY_VALUE(PendingRecruitCharacterId, InCharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(PendingRecruitCharacterIdValue, InCharacterId.Value);

	// Everything else about the pick — which row is highlighted, what the gate says — is derived
	// from the value just written, by the refresh below.
	RefreshModalValidation();
}

void UPlayerCharacterViewModel::HandleRecruitableCharacterClicked(USelectableCharacterViewModel* ClickedCharacter)
{
	if (ClickedCharacter == nullptr)
	{
		return;
	}

	// Clicking the picked row unpicks it, same toggle shape as HandleJoinableOrganizationClicked.
	const FCharacterId ClickedId = ClickedCharacter->GetCharacterId();
	ApplyRecruitTarget(ClickedId == PendingRecruitCharacterId ? FCharacterId() : ClickedId);
}

void UPlayerCharacterViewModel::RefreshJoinableOrganizations()
{
	UDepartments* DepartmentsContainer = FindRtsGameStateComponent<UDepartments>(this);
	UBuildings* BuildingsContainer = FindRtsGameStateComponent<UBuildings>(this);

	// Not named CurrentProvinceId: UCharacterViewModel already has a protected field of that name,
	// and this project builds with shadow-variable warnings as errors.
	const FProvinceId CharacterProvinceId = Characters ? Characters->GetCurrentProvince(GetCharacterId()) : FProvinceId();
	const TArray<FOrganizationId> PresentIds = URtsPresence::CollectOrganizationsIn(CharacterProvinceId, DepartmentsContainer, BuildingsContainer);

	TArray<TObjectPtr<USelectableOrganizationViewModel>> NewRows;
	USelectableOrganizationViewModel* NewSelectedRow = nullptr;

	for (const FOrganizationId Id : PresentIds)
	{
		// Already a member — nothing to join here, see HoldsAnyPositionAt's counterpart in
		// RtsActions.cpp for the same check on the execute side.
		bool bAlreadyMember = false;
		for (const UPositionViewModel* Position : GetPositions())
		{
			if (Position && Position->GetOrganizationId() == Id)
			{
				bAlreadyMember = true;
				break;
			}
		}
		if (bAlreadyMember)
		{
			continue;
		}

		TObjectPtr<USelectableOrganizationViewModel>& Entry = JoinableOrganizationCache.FindOrAdd(Id);
		if (Entry == nullptr)
		{
			Entry = NewObject<USelectableOrganizationViewModel>(this);
			Entry->OnClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandleJoinableOrganizationClicked);
		}

		const FOrganizationInfo Info = Organizations ? Organizations->GetOrganizationInfo(Id) : FOrganizationInfo();
		Entry->SetSource(Id, Info.OrganizationName, Info.Logo);

		if (Id == PendingJoinOrganizationId)
		{
			NewSelectedRow = Entry;
		}

		NewRows.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(JoinableOrganizations, NewRows);
	UE_MVVM_SET_PROPERTY_VALUE(JoinableOrganizationCount, NewRows.Num());
	UE_MVVM_SET_PROPERTY_VALUE(SelectedJoinOrganization, NewSelectedRow);
}

void UPlayerCharacterViewModel::RefreshRecruitableCharacters()
{
	// Recruiting is a RAS_Position action — meaningless while acting as self, so the list is empty
	// then rather than guessing which organization is meant.
	if (bIsActingAsSelf || Characters == nullptr)
	{
		UE_MVVM_SET_PROPERTY_VALUE(RecruitableCharacters, TArray<TObjectPtr<USelectableCharacterViewModel>>());
		UE_MVVM_SET_PROPERTY_VALUE(RecruitableCharacterCount, 0);
		UE_MVVM_SET_PROPERTY_VALUE(SelectedRecruitCharacter, nullptr);
		return;
	}

	const UPositionViewModel* ActingPosition = GetPositions().IsValidIndex(SelectedPositionIndex) ? GetPositions()[SelectedPositionIndex].Get() : nullptr;
	const FOrganizationId ActingOrganizationId = ActingPosition ? ActingPosition->GetOrganizationId() : FOrganizationId();

	UDepartments* DepartmentsContainer = FindRtsGameStateComponent<UDepartments>(this);
	UBuildings* BuildingsContainer = FindRtsGameStateComponent<UBuildings>(this);

	const FProvinceId SeatProvinceId = URtsPresence::FindSeatProvince(ActingOrganizationId, Organizations, DepartmentsContainer, BuildingsContainer);
	const TArray<FCharacterId> CandidateIds = SeatProvinceId.IsValid() ? Characters->CollectCharactersInProvince(SeatProvinceId) : TArray<FCharacterId>();

	TArray<TObjectPtr<USelectableCharacterViewModel>> NewRows;
	USelectableCharacterViewModel* NewSelectedRow = nullptr;

	for (const FCharacterId CandidateId : CandidateIds)
	{
		// Already holds a position here — nothing to recruit, same check the recruit input gate makes.
		bool bAlreadyMember = false;
		for (const FPosition& Position : Characters->GetPositions(CandidateId))
		{
			if (Position.OrganizationId == ActingOrganizationId)
			{
				bAlreadyMember = true;
				break;
			}
		}
		if (bAlreadyMember)
		{
			continue;
		}

		TObjectPtr<USelectableCharacterViewModel>& Entry = RecruitableCharacterCache.FindOrAdd(CandidateId);
		if (Entry == nullptr)
		{
			Entry = NewObject<USelectableCharacterViewModel>(this);
			Entry->OnClicked.AddDynamic(this, &UPlayerCharacterViewModel::HandleRecruitableCharacterClicked);
		}

		const FCharacterDisplayInfo Display = Characters->GetDisplayInfo(CandidateId);
		Entry->SetSource(CandidateId, Display.CharacterName, Display.Portrait);

		if (CandidateId == PendingRecruitCharacterId)
		{
			NewSelectedRow = Entry;
		}

		NewRows.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(RecruitableCharacters, NewRows);
	UE_MVVM_SET_PROPERTY_VALUE(RecruitableCharacterCount, NewRows.Num());
	UE_MVVM_SET_PROPERTY_VALUE(SelectedRecruitCharacter, NewSelectedRow);
}

void UPlayerCharacterViewModel::RefreshModalValidation()
{
	// The rows carry prices and affordability, both of which move with the character's money — so
	// they are re-derived wherever the gate is, not only when the modal opens.
	RefreshJoinableOrganizations();
	RefreshRecruitableCharacters();

	// PM_ViewProvince only displays a UProvinceViewModel and has no action to gate — there is
	// nothing here for it to confirm.
	if (!IsActionModal(ActiveModal))
	{
		UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, FText::GetEmpty());
		UE_MVVM_SET_PROPERTY_VALUE(bCanConfirmModal, false);
		return;
	}

	const FText Reason = ComputeModalError();
	UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, Reason);
	UE_MVVM_SET_PROPERTY_VALUE(bCanConfirmModal, Reason.IsEmpty());
}

FRtsActionInput UPlayerCharacterViewModel::MakeInput() const
{
	// Covers only the join/recruit modals now — the found-organization form builds its own
	// FRtsActionInput in HandleFoundOrganizationConfirmed from what it collected itself.
	FRtsActionInput Input;
	Input.TargetOrganizationId = PendingJoinOrganizationId;
	Input.TargetCharacterId = PendingRecruitCharacterId;
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
	Context.Buildings = FindRtsGameStateComponent<UBuildings>(this);
	Context.Departments = FindRtsGameStateComponent<UDepartments>(this);
	Context.Banking = FindRtsGameStateComponent<UBanking>(this);
	Context.Provinces = FindRtsGameStateComponent<UProvinces>(this);

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
	// here directly — the same call every other facade lookup goes through.
	Context.Banking = FindRtsGameStateComponent<UBanking>(this);
	Context.Departments = FindRtsGameStateComponent<UDepartments>(this);

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

	// Which scope a button belongs to is what decides whether it is on screen at all; its own gate
	// then decides whether it is clickable, and says why not when it is not. Written out per action
	// rather than looped over a catalog — every action is known here by name.
	const FText FoundOrganizationReason = URtsActions::GetFoundOrganizationBlockedReason(Context);
	UE_MVVM_SET_PROPERTY_VALUE(bIsFoundOrganizationVisible, bIsActingAsSelf);
	UE_MVVM_SET_PROPERTY_VALUE(bIsFoundOrganizationEnabled, FoundOrganizationReason.IsEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(FoundOrganizationTooltip, FoundOrganizationReason);

	const FText JoinOrganizationReason = URtsActions::GetJoinOrganizationBlockedReason(Context);
	UE_MVVM_SET_PROPERTY_VALUE(bIsJoinOrganizationVisible, bIsActingAsSelf);
	UE_MVVM_SET_PROPERTY_VALUE(bIsJoinOrganizationEnabled, JoinOrganizationReason.IsEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(JoinOrganizationTooltip, JoinOrganizationReason);

	const FText LeaveOrganizationReason = URtsActions::GetLeaveOrganizationBlockedReason(Context);
	UE_MVVM_SET_PROPERTY_VALUE(bIsLeaveOrganizationVisible, !bIsActingAsSelf);
	UE_MVVM_SET_PROPERTY_VALUE(bIsLeaveOrganizationEnabled, LeaveOrganizationReason.IsEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(LeaveOrganizationTooltip, LeaveOrganizationReason);

	const FText RecruitCharacterReason = URtsActions::GetRecruitCharacterBlockedReason(Context);
	UE_MVVM_SET_PROPERTY_VALUE(bIsRecruitCharacterVisible, !bIsActingAsSelf);
	UE_MVVM_SET_PROPERTY_VALUE(bIsRecruitCharacterEnabled, RecruitCharacterReason.IsEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(RecruitCharacterTooltip, RecruitCharacterReason);
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

// Each of the four re-runs its own gate instead of trusting whichever bIsXxxEnabled was last pushed
// into the button — the same reason a React handler does not trust the rendered `disabled`. Actions
// that need input from the player open their modal instead of running, and ConfirmModal finishes
// the job; which modal that is stays here rather than in URtsActions, because EPlayerModal is
// UI-invented state a pure-derivation view model is not allowed to know about.

void UPlayerCharacterViewModel::ClickFoundOrganization()
{
	if (!URtsActions::GetFoundOrganizationBlockedReason(MakeContext()).IsEmpty())
	{
		return;
	}

	ToggleModal(EPlayerModal::PM_FoundOrganization);
}

void UPlayerCharacterViewModel::ClickJoinOrganization()
{
	if (!URtsActions::GetJoinOrganizationBlockedReason(MakeContext()).IsEmpty())
	{
		return;
	}

	ToggleModal(EPlayerModal::PM_JoinOrganization);
}

void UPlayerCharacterViewModel::ClickLeaveOrganization()
{
	const FRtsActionContext Context = MakeContext();
	if (!URtsActions::GetLeaveOrganizationBlockedReason(Context).IsEmpty())
	{
		return;
	}

	// No input to collect — this one runs straight from the click. The refresh that follows
	// (OnPositionChanged -> Characters event -> Refresh_Implementation) rebuilds the selection and
	// the action buttons.
	URtsActions::ExecuteLeaveOrganization(Context);
}

void UPlayerCharacterViewModel::ClickRecruitCharacter()
{
	if (!URtsActions::GetRecruitCharacterBlockedReason(MakeContext()).IsEmpty())
	{
		return;
	}

	ToggleModal(EPlayerModal::PM_RecruitCharacter);
}

void UPlayerCharacterViewModel::ToggleModal(const EPlayerModal InModal)
{
	// Clicking the action again while its modal is already open closes it, the same toggle shape
	// HandlePositionClicked uses for its own selection.
	if (ActiveModal == InModal)
	{
		CloseModal();
		return;
	}

	OpenModal(InModal);
}

void UPlayerCharacterViewModel::BindBanking()
{
	UBanking* NewBanking = FindRtsGameStateComponent<UBanking>(this);
	if (Banking == NewBanking)
	{
		return;
	}

	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}

	Banking = NewBanking;

	if (Banking)
	{
		Banking->OnBalanceChanged.AddDynamic(this, &UPlayerCharacterViewModel::HandleBalanceChanged);
	}
}

void UPlayerCharacterViewModel::HandleBalanceChanged(const FBankAccountId InAccountId)
{
	// Unfiltered — this fires for every account's balance change, not just this character's or their
	// organization's. Re-deriving both is cheap and the same tolerance ProvinceViewModel/
	// OrganizationViewModel already have for their own unfiltered facade events.
	RefreshResources();
	RefreshModalValidation();
}

void UPlayerCharacterViewModel::BeginDestroy()
{
	if (Player)
	{
		Player->OnControlledCharacterIdChanged.RemoveAll(this);
	}

	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}

	Super::BeginDestroy();
}
