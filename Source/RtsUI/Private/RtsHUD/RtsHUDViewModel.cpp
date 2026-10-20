#include "RtsHUD/RtsHUDViewModel.h"

#include "Actions/RtsActions.h"
#include "Banking.h"
#include "BuildingCatalog.h"
#include "Buildings.h"
#include "BuildingType.h"
#include "CharacterInfo.h"
#include "Characters.h"
#include "Characters/CharacterViewModel.h"
#include "Characters/SelectableCharacterViewModel.h"
#include "Departments.h"
#include "Focuses.h"
#include "Focuses/RtsFocuses.h"
#include "Focuses/UseOrganizationFocusTree.h"
#include "Kismet/GameplayStatics.h"
#include "MapModes/UseWorldMapView.h"
#include "OrganizationInfo.h"
#include "Organizations.h"
#include "Organizations/SelectableOrganizationViewModel.h"
#include "Organizations/SelectablePositionViewModel.h"
#include "Presence/RtsPresence.h"
#include "Provinces.h"
#include "Resources/ResourceViewModel.h"
#include "Resources/RtsResources.h"
#include "Roles.h"
#include "RtsGameStateComponent.h"
#include "RtsMapPawn.h"
#include "RtsPlayer.h"
#include "Time.h"

URtsHUDViewModel::URtsHUDViewModel()
{

}

void URtsHUDViewModel::Initialize()
{
	UnbindFacades();

	Provinces = FindRtsGameStateComponent<UProvinces>(this);
	Organizations = FindRtsGameStateComponent<UOrganizations>(this);
	Departments = FindRtsGameStateComponent<UDepartments>(this);
	Buildings = FindRtsGameStateComponent<UBuildings>(this);
	Banking = FindRtsGameStateComponent<UBanking>(this);
	Characters = FindRtsGameStateComponent<UCharacters>(this);
	Roles = FindRtsGameStateComponent<URoles>(this);
	OrganizationFocuses = FindRtsGameStateComponent<UOrganizationFocuses>(this);
	Time = FindRtsGameStateComponent<UTime>(this);

	if (PlayedCharacter == nullptr)
	{
		PlayedCharacter = NewObject<UCharacterViewModel>(this);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayedCharacter);
	}

	if (MapView == nullptr)
	{
		MapView = NewObject<UUseWorldMapView>(this);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MapView);
	}
	MapView->Initialize();

	if (FocusTree == nullptr)
	{
		FocusTree = NewObject<UUseOrganizationFocusTree>(this);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(FocusTree);
	}
	FocusTree->Initialize();

	BindFacades();
	RefreshBuildMenu();

	// Last, because it pulls the controlled character in and everything derived from it expects the
	// facades above to be resolved already.
	BindPlayer();
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

void URtsHUDViewModel::SetSelectedProvinceId(const FProvinceId InSelectedProvinceId)
{
	const bool bIsAlive = Provinces && Provinces->IsAlive(InSelectedProvinceId);

	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceId, InSelectedProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceIdValue, InSelectedProvinceId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceName,
		bIsAlive ? Provinces->GetDisplayInfo(InSelectedProvinceId).Name : FText::GetEmpty());

	BroadcastFoundOrganizationVerdict();

	// While the player is filling in the found-organization form, a map click is that form's site
	// field. Opening the province modal over it would throw the half-filled form away, so the
	// selection lands and nothing else moves.
	if (ActiveModal == EPlayerModal::PM_FoundOrganization)
	{
		return;
	}

	if (!InSelectedProvinceId.IsValid())
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

	USoundBase* InspectionSound = bIsAlive ? Provinces->GetDisplayInfo(InSelectedProvinceId).InspectionSound : nullptr;
	if (InspectionSound)
	{
		UGameplayStatics::PlaySound2D(this, InspectionSound);
	}
}

//~ Position selection ------------------------------------------------------------------------------

void URtsHUDViewModel::SelectSelf()
{
	ApplySelection(INDEX_NONE);
}

void URtsHUDViewModel::SelectPosition(const int32 InPositionIndex)
{
	if (!SelectablePositions.IsValidIndex(InPositionIndex))
	{
		return;
	}

	ApplySelection(InPositionIndex);
}

void URtsHUDViewModel::ApplySelection(const int32 InPositionIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, InPositionIndex);
	UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, InPositionIndex == INDEX_NONE);

	USelectablePositionViewModel* NewSelectedPosition =
		SelectablePositions.IsValidIndex(InPositionIndex) ? SelectablePositions[InPositionIndex].Get() : nullptr;
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPosition, NewSelectedPosition);

	RefreshActions();
	RefreshResources();
	RefreshFocusTree();
}

void URtsHUDViewModel::RefreshSelectablePositions()
{
	const TArray<FPosition> CharacterPositions =
		Characters ? Characters->GetPositions(ControlledCharacterId) : TArray<FPosition>();

	TArray<TObjectPtr<USelectablePositionViewModel>> NewSelectablePositions;
	NewSelectablePositions.Reserve(CharacterPositions.Num());

	for (int32 Index = 0; Index < CharacterPositions.Num(); ++Index)
	{
		if (!SelectablePositionCache.IsValidIndex(Index))
		{
			USelectablePositionViewModel* NewEntry = NewObject<USelectablePositionViewModel>(this);
			NewEntry->OnClicked.AddDynamic(this, &URtsHUDViewModel::HandlePositionClicked);
			SelectablePositionCache.Add(NewEntry);
		}

		const FOrganizationId OrganizationId = CharacterPositions[Index].OrganizationId;
		const FOrganizationInfo Info = Organizations ? Organizations->GetOrganizationInfo(OrganizationId) : FOrganizationInfo();

		USelectablePositionViewModel* Entry = SelectablePositionCache[Index];
		Entry->SetSource(Index, OrganizationId, Info.OrganizationName, Info.Logo);
		NewSelectablePositions.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(SelectablePositions, NewSelectablePositions);
	UE_MVVM_SET_PROPERTY_VALUE(SelectablePositionCount, NewSelectablePositions.Num());
}

void URtsHUDViewModel::HandlePositionClicked(USelectablePositionViewModel* ClickedPosition)
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

void URtsHUDViewModel::RefreshPlayedCharacter()
{
	// Captured before the picker is rebuilt: which organization (not which index) was selected. A
	// position lower in the list can disappear — leaving an organization removes one — which shifts
	// every later index down. Re-deriving the selection by index alone would silently land on a
	// different organization's position; identity is what actually has to survive a refresh.
	const FOrganizationId PreviouslySelectedOrganizationId =
		!bIsActingAsSelf && SelectedPosition ? SelectedPosition->GetOrganizationId() : FOrganizationId();

	// Without the facade nothing below can be trusted, and a modal belongs to the character that
	// opened it rather than staying open over someone else.
	if (Characters == nullptr)
	{
		CloseModal();
	}

	// The picker is rebuilt first so the selection below can point at a current row.
	RefreshSelectablePositions();

	// A position the character no longer holds (or never held, on the first refresh) falls back to
	// acting as self rather than pointing at a stale index. Reapplying an unchanged selection also
	// re-gates the action list against the values that just changed.
	int32 NewSelectedPositionIndex = INDEX_NONE;
	if (PreviouslySelectedOrganizationId.IsValid())
	{
		for (int32 Index = 0; Index < SelectablePositions.Num(); ++Index)
		{
			if (SelectablePositions[Index] && SelectablePositions[Index]->GetOrganizationId() == PreviouslySelectedOrganizationId)
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

void URtsHUDViewModel::HandlePlayedCharacterEvent(const FCharacterId InCharacterId)
{
	if (InCharacterId == ControlledCharacterId)
	{
		RefreshPlayedCharacter();
	}
}

const FPosition* URtsHUDViewModel::FindSelectedPosition(const TArray<FPosition>& InPositions) const
{
	if (bIsActingAsSelf || !InPositions.IsValidIndex(SelectedPositionIndex))
	{
		return nullptr;
	}

	return &InPositions[SelectedPositionIndex];
}

//~ Modals ------------------------------------------------------------------------------------------

void URtsHUDViewModel::OpenModal(const EPlayerModal InModal)
{
	ApplyModal(InModal);
}

void URtsHUDViewModel::CloseModal()
{
	ApplyModal(EPlayerModal::PM_None);
}

void URtsHUDViewModel::ApplyModal(const EPlayerModal InModal)
{
	UE_MVVM_SET_PROPERTY_VALUE(ActiveModal, InModal);
	UE_MVVM_SET_PROPERTY_VALUE(bIsFoundOrganizationModalOpen, InModal == EPlayerModal::PM_FoundOrganization);
	UE_MVVM_SET_PROPERTY_VALUE(bIsJoinOrganizationModalOpen, InModal == EPlayerModal::PM_JoinOrganization);
	UE_MVVM_SET_PROPERTY_VALUE(bIsViewProvinceModalOpen, InModal == EPlayerModal::PM_ViewProvince);
	UE_MVVM_SET_PROPERTY_VALUE(bIsViewOrganizationModalOpen, InModal == EPlayerModal::PM_ViewOrganization);
	UE_MVVM_SET_PROPERTY_VALUE(bIsRecruitCharacterModalOpen, InModal == EPlayerModal::PM_RecruitCharacter);
	UE_MVVM_SET_PROPERTY_VALUE(bIsFocusTreeModalOpen, InModal == EPlayerModal::PM_FocusTree);

	// The build menu belongs to whichever process needs a building picked, and today that is
	// founding an organization - so the modal opening brings the one menu up and closing it takes
	// it down again, pick and all.
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeIsBuildMenuOpen);
	ApplySelectedBuildingType(nullptr);
	RefreshBuildMenu();

	// The typed name goes with the form it was typed into.
	if (InModal != EPlayerModal::PM_FoundOrganization)
	{
		SetPendingOrganizationName(FText::GetEmpty());
	}

	// Whatever the last modal collected is not this one's starting point. The province is not reset
	// with it: that is where the player is looking on the map, not something this modal collected —
	// and starting the form on the province they were just inspecting is the useful default. The
	// found-organization form resets itself (see UFoundOrganizationModalWidgetViewModel::ResetForm),
	// there being no state of its own left here to clear.
	ApplyJoinTarget(FOrganizationId());
	ApplyRecruitTarget(FCharacterId());

	RefreshMapOverlay();
}

void URtsHUDViewModel::ToggleModal(const EPlayerModal InModal)
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

void URtsHUDViewModel::ViewOrganization(const FOrganizationId InOrganizationId)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedOrganizationId, InOrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedOrganizationIdValue, InOrganizationId.Value);
	OpenModal(EPlayerModal::PM_ViewOrganization);
}

void URtsHUDViewModel::ConfirmModal()
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

void URtsHUDViewModel::HandleFoundOrganizationConfirmed(const FText& InOrganizationName, UBuildingType* InHeadquartersType, const FProvinceId InProvinceId)
{
	// Re-run both gates on exactly what the form reported rather than trusting its own bIsValid —
	// the same rule ConfirmModal follows for the other two modals.
	const FRtsActionContext Context = MakeFoundOrganizationContext();

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

FText URtsHUDViewModel::ComputeModalError() const
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

bool URtsHUDViewModel::IsActionModal(const EPlayerModal InModal) const
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

void URtsHUDViewModel::RefreshModalValidation()
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

void URtsHUDViewModel::SelectJoinTarget(const FOrganizationId InOrganizationId)
{
	ApplyJoinTarget(InOrganizationId);
}

void URtsHUDViewModel::ApplyJoinTarget(const FOrganizationId InOrganizationId)
{
	UE_MVVM_SET_PROPERTY_VALUE(PendingJoinOrganizationId, InOrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(PendingJoinOrganizationIdValue, InOrganizationId.Value);

	// Everything else about the pick — which row is highlighted, what the gate says — is derived
	// from the value just written, by the refresh below.
	RefreshModalValidation();
}

void URtsHUDViewModel::HandleJoinableOrganizationClicked(USelectableOrganizationViewModel* ClickedOrganization)
{
	if (ClickedOrganization == nullptr)
	{
		return;
	}

	// Clicking the picked row unpicks it, exactly as clicking the selected position row does.
	const FOrganizationId ClickedId = ClickedOrganization->GetOrganizationId();
	ApplyJoinTarget(ClickedId == PendingJoinOrganizationId ? FOrganizationId() : ClickedId);
}

void URtsHUDViewModel::RefreshJoinableOrganizations()
{
	const FProvinceId CharacterProvinceId = Characters ? Characters->GetCurrentProvince(ControlledCharacterId) : FProvinceId();
	const TArray<FPosition> CharacterPositions =
		Characters ? Characters->GetPositions(ControlledCharacterId) : TArray<FPosition>();
	const TArray<FOrganizationId> PresentIds = URtsPresence::CollectOrganizationsIn(CharacterProvinceId, Departments, Buildings);

	TArray<TObjectPtr<USelectableOrganizationViewModel>> NewRows;
	USelectableOrganizationViewModel* NewSelectedRow = nullptr;

	for (const FOrganizationId Id : PresentIds)
	{
		// Already a member — nothing to join here, see HoldsAnyPositionAt's counterpart in
		// RtsActions.cpp for the same check on the execute side.
		bool bAlreadyMember = false;
		for (const FPosition& Position : CharacterPositions)
		{
			if (Position.OrganizationId == Id)
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
			Entry->OnClicked.AddDynamic(this, &URtsHUDViewModel::HandleJoinableOrganizationClicked);
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

void URtsHUDViewModel::SelectRecruitTarget(const FCharacterId InCharacterId)
{
	ApplyRecruitTarget(InCharacterId);
}

void URtsHUDViewModel::ApplyRecruitTarget(const FCharacterId InCharacterId)
{
	UE_MVVM_SET_PROPERTY_VALUE(PendingRecruitCharacterId, InCharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(PendingRecruitCharacterIdValue, InCharacterId.Value);

	// Everything else about the pick — which row is highlighted, what the gate says — is derived
	// from the value just written, by the refresh below.
	RefreshModalValidation();
}

void URtsHUDViewModel::HandleRecruitableCharacterClicked(USelectableCharacterViewModel* ClickedCharacter)
{
	if (ClickedCharacter == nullptr)
	{
		return;
	}

	// Clicking the picked row unpicks it, same toggle shape as HandleJoinableOrganizationClicked.
	const FCharacterId ClickedId = ClickedCharacter->GetCharacterId();
	ApplyRecruitTarget(ClickedId == PendingRecruitCharacterId ? FCharacterId() : ClickedId);
}

void URtsHUDViewModel::RefreshRecruitableCharacters()
{
	// Recruiting is a RAS_Position action — meaningless while acting as self, so the list is empty
	// then rather than guessing which organization is meant.
	const TArray<FPosition> CharacterPositions =
		Characters && !bIsActingAsSelf ? Characters->GetPositions(ControlledCharacterId) : TArray<FPosition>();
	const FPosition* ActingPosition = FindSelectedPosition(CharacterPositions);

	if (ActingPosition == nullptr || Characters == nullptr)
	{
		UE_MVVM_SET_PROPERTY_VALUE(RecruitableCharacters, TArray<TObjectPtr<USelectableCharacterViewModel>>());
		UE_MVVM_SET_PROPERTY_VALUE(RecruitableCharacterCount, 0);
		UE_MVVM_SET_PROPERTY_VALUE(SelectedRecruitCharacter, nullptr);
		return;
	}

	const FOrganizationId ActingOrganizationId = ActingPosition->OrganizationId;
	const FProvinceId SeatProvinceId = URtsPresence::FindSeatProvince(ActingOrganizationId, Organizations, Departments, Buildings);
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
			Entry->OnClicked.AddDynamic(this, &URtsHUDViewModel::HandleRecruitableCharacterClicked);
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

//~ Actions and resources ---------------------------------------------------------------------------

FRtsActionContext URtsHUDViewModel::MakeContext() const
{
	FRtsActionContext Context;
	Context.ActorId = ControlledCharacterId;
	Context.Scope = bIsActingAsSelf ? ERtsActionScope::RAS_Self : ERtsActionScope::RAS_Position;
	Context.Characters = Characters;
	Context.Organizations = Organizations;
	Context.Roles = Roles;
	Context.Buildings = Buildings;
	Context.Departments = Departments;
	Context.Banking = Banking;
	Context.Provinces = Provinces;
	Context.OrganizationFocuses = OrganizationFocuses;
	Context.Time = Time;

	if (Characters && !bIsActingAsSelf)
	{
		// Organization and role are read from the same position, so the context can never claim a
		// role at an organization the character does not hold it at.
		const TArray<FPosition> CharacterPositions = Characters->GetPositions(ControlledCharacterId);
		if (const FPosition* Position = FindSelectedPosition(CharacterPositions))
		{
			Context.OrganizationId = Position->OrganizationId;
			Context.RoleId = Position->RoleId;
		}
	}

	return Context;
}

FRtsActionInput URtsHUDViewModel::MakeInput() const
{
	// Covers only the join/recruit modals — the found-organization form builds its own
	// FRtsActionInput in MakeFoundOrganizationInput from what it collected itself.
	FRtsActionInput Input;
	Input.TargetOrganizationId = PendingJoinOrganizationId;
	Input.TargetCharacterId = PendingRecruitCharacterId;
	return Input;
}

void URtsHUDViewModel::RefreshActions()
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

void URtsHUDViewModel::RefreshFocusTree()
{
	if (FocusTree == nullptr)
	{
		return;
	}

	FocusTree->SetOrganizationId(MakeContext().OrganizationId);
}

void URtsHUDViewModel::StartFocus(UOrganizationFocusAsset* InFocus)
{
	const FRtsActionContext Context = MakeContext();
	if (!URtsFocuses::GetStartFocusBlockedReason(Context, InFocus).IsEmpty())
	{
		return;
	}

	URtsFocuses::ExecuteStartFocus(Context, InFocus);
}

void URtsHUDViewModel::ChooseFocusEventOption(const int32 InOptionIndex)
{
	const FRtsActionContext Context = MakeContext();
	if (!URtsFocuses::GetChooseEventOptionBlockedReason(Context, InOptionIndex).IsEmpty())
	{
		return;
	}

	URtsFocuses::ExecuteChooseEventOption(Context, InOptionIndex);
}

FRtsResourceContext URtsHUDViewModel::MakeResourceContext() const
{
	FRtsResourceContext Context;
	Context.Characters = Characters;
	Context.Organizations = Organizations;
	Context.Banking = Banking;
	Context.Departments = Departments;

	if (bIsActingAsSelf || Characters == nullptr)
	{
		Context.Owner = FResourceOwner::ForCharacter(ControlledCharacterId);
		return Context;
	}

	// Same lookup MakeContext uses for OrganizationId — the selected position's organization is
	// the resource owner while acting on its behalf.
	const TArray<FPosition> CharacterPositions = Characters->GetPositions(ControlledCharacterId);
	const FPosition* Position = FindSelectedPosition(CharacterPositions);
	if (Position && Position->OrganizationId.IsValid())
	{
		Context.Owner = FResourceOwner::ForOrganization(Position->OrganizationId);
	}

	return Context;
}

void URtsHUDViewModel::RefreshResources()
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
			Entry->OnClicked.AddDynamic(this, &URtsHUDViewModel::HandleResourceClicked);
		}

		Entry->SetSource(ResourceType, Context);
		Entry->SetExpanded(bHasExpandedResource && ResourceType == ExpandedResourceType);
		NewResources.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Resources, NewResources);
	UE_MVVM_SET_PROPERTY_VALUE(ResourceCount, NewResources.Num());
}

void URtsHUDViewModel::HandleResourceClicked(UResourceViewModel* ClickedResource)
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
// the job.

void URtsHUDViewModel::ClickFoundOrganization()
{
	if (!URtsActions::GetFoundOrganizationBlockedReason(MakeContext()).IsEmpty())
	{
		return;
	}

	ToggleModal(EPlayerModal::PM_FoundOrganization);
}

void URtsHUDViewModel::ClickJoinOrganization()
{
	if (!URtsActions::GetJoinOrganizationBlockedReason(MakeContext()).IsEmpty())
	{
		return;
	}

	ToggleModal(EPlayerModal::PM_JoinOrganization);
}

void URtsHUDViewModel::ClickLeaveOrganization()
{
	const FRtsActionContext Context = MakeContext();
	if (!URtsActions::GetLeaveOrganizationBlockedReason(Context).IsEmpty())
	{
		return;
	}

	// No input to collect — this one runs straight from the click. The refresh that follows
	// (OnPositionChanged -> HandlePlayedCharacterEvent) rebuilds the selection and the action
	// buttons.
	URtsActions::ExecuteLeaveOrganization(Context);
}

void URtsHUDViewModel::ClickRecruitCharacter()
{
	if (!URtsActions::GetRecruitCharacterBlockedReason(MakeContext()).IsEmpty())
	{
		return;
	}

	ToggleModal(EPlayerModal::PM_RecruitCharacter);
}

//~ Build menu --------------------------------------------------------------------------------------

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

	RefreshMapOverlay();
	BroadcastFoundOrganizationVerdict();
}

void URtsHUDViewModel::RefreshMapOverlay()
{
	if (MapView)
	{
		MapView->SetFoundingSiteOverlay(ControlledCharacterId, ComputeIsBuildMenuOpen() ? SelectedBuildingType : nullptr);
	}
}

void URtsHUDViewModel::RefreshBuildMenu()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ComputeBuildings);

	// The filter can drop the picked kind out of the menu entirely, and a pick nobody can see any
	// more is the same stale state closing the modal clears.
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
	// Unfiltered: this fires for every account, not just the played character's or their
	// organization's. Re-deriving is cheap and affordability has to stay honest while a menu or a
	// modal is open.
	RefreshBuildMenu();
	BroadcastFoundOrganizationVerdict();
	RefreshResources();
	RefreshModalValidation();
}

//~ Found organization ------------------------------------------------------------------------------

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
	FRtsActionContext Context = MakeContext();
	Context.Scope = ERtsActionScope::RAS_Self;
	Context.OrganizationId = FOrganizationId();
	Context.RoleId = FRoleId();
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

//~ Facade events -----------------------------------------------------------------------------------

void URtsHUDViewModel::BindFacades()
{
	if (Banking)
	{
		Banking->OnBalanceChanged.AddDynamic(this, &URtsHUDViewModel::HandleBalanceChanged);
	}

	// Everything derived from the played character — the position picker, the action gates, the
	// resource rows — filtered to the controlled id in HandlePlayedCharacterEvent.
	if (Characters)
	{
		Characters->OnCurrentProvinceChanged.AddDynamic(this, &URtsHUDViewModel::HandlePlayedCharacterEvent);
		Characters->OnPositionChanged.AddDynamic(this, &URtsHUDViewModel::HandlePlayedCharacterEvent);
		Characters->OnSimulated.AddDynamic(this, &URtsHUDViewModel::HandlePlayedCharacterEvent);
		Characters->OnCharacterDestroyed.AddDynamic(this, &URtsHUDViewModel::HandlePlayedCharacterEvent);
	}
}

void URtsHUDViewModel::UnbindFacades()
{
	if (Banking)
	{
		Banking->OnBalanceChanged.RemoveAll(this);
	}

	if (Characters)
	{
		Characters->OnCurrentProvinceChanged.RemoveAll(this);
		Characters->OnPositionChanged.RemoveAll(this);
		Characters->OnSimulated.RemoveAll(this);
		Characters->OnCharacterDestroyed.RemoveAll(this);
	}
}

//~ The played character ----------------------------------------------------------------------------

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
		RefreshMapOverlay();
	}

	if (PlayedCharacter)
	{
		PlayedCharacter->Initialize(ControlledCharacterId);
	}

	RefreshPlayedCharacter();
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
