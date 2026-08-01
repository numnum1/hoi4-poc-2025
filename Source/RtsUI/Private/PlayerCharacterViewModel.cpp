#include "PlayerCharacterViewModel.h"

#include "ActionViewModel.h"
#include "Characters.h"
#include "PositionViewModel.h"
#include "RtsActions.h"
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
	Super::Refresh_Implementation();

	if (Characters == nullptr)
	{
		UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, INDEX_NONE);
		UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, true);
		// A modal belongs to the character that opened it; switching away drops it rather than
		// leaving a dialog open over someone else.
		CloseModal();
	}
	// A position the character no longer holds (or never held, on the first refresh) falls back to
	// acting as self rather than pointing at a stale index.
	else if (SelectedPositionIndex != INDEX_NONE && !GetPositions().IsValidIndex(SelectedPositionIndex))
	{
		UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, INDEX_NONE);
		UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, true);
	}

	RefreshActions();
}

void UPlayerCharacterViewModel::SelectSelf()
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, true);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, INDEX_NONE);
	RefreshActions();
}

void UPlayerCharacterViewModel::SelectPosition(const int32 InPositionIndex)
{
	if (!GetPositions().IsValidIndex(InPositionIndex))
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, false);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, InPositionIndex);
	RefreshActions();
}

void UPlayerCharacterViewModel::OpenModal(const EPlayerModal InModal)
{
	UE_MVVM_SET_PROPERTY_VALUE(ActiveModal, InModal);
	UE_MVVM_SET_PROPERTY_VALUE(bIsFoundOrganizationModalOpen, InModal == EPlayerModal::PM_FoundOrganization);

	// Whatever the last modal collected is not this one's starting point.
	UE_MVVM_SET_PROPERTY_VALUE(PendingOrganizationName, FText::GetEmpty());
	RefreshModalValidation();
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

void UPlayerCharacterViewModel::ConfirmModal()
{
	if (ActiveModal == EPlayerModal::PM_None)
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

void UPlayerCharacterViewModel::RefreshModalValidation()
{
	if (ActiveModal == EPlayerModal::PM_None)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ModalErrorText, FText::GetEmpty());
		UE_MVVM_SET_PROPERTY_VALUE(bCanConfirmModal, false);
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
}

FRtsActionInput UPlayerCharacterViewModel::MakeInput() const
{
	FRtsActionInput Input;
	Input.OrganizationName = PendingOrganizationName;
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
		FPosition SelectedPosition;
		SelectedPosition.OrganizationId = Context.OrganizationId;
		SelectedPosition.RoleId = Context.RoleId;
		OfferedActions = URtsActions::GetActionsForPosition(SelectedPosition);
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
