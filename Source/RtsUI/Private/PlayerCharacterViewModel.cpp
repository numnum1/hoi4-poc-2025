#include "PlayerCharacterViewModel.h"

#include "ActionViewModel.h"
#include "CharacterProxy.h"
#include "Characters.h"
#include "OrganizationProxy.h"
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
	// Inherited from UCharacterViewModel — resolves the proxy, rebinds OnChanged, and calls
	// Refresh(), which the override below drives the selection and the action list from.
	Initialize(Player ? Player->GetControlledCharacterId() : FCharacterId());
}

void UPlayerCharacterViewModel::Refresh_Implementation()
{
	Super::Refresh_Implementation();

	if (Proxy == nullptr)
	{
		UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, INDEX_NONE);
		UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, true);
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

FRtsActionContext UPlayerCharacterViewModel::MakeContext() const
{
	FRtsActionContext Context;
	Context.Actor = Proxy;
	Context.Scope = bIsActingAsSelf ? ERtsActionScope::RAS_Self : ERtsActionScope::RAS_Position;

	if (!bIsActingAsSelf)
	{
		// Organization and role are read from the same position index, so the context can never
		// claim a role at an organization the character does not hold it at.
		const UOrganizationProxy* Organization = Proxy ? Proxy->GetOrganization(SelectedPositionIndex) : nullptr;
		Context.OrganizationId = Organization ? Organization->GetId() : FOrganizationId();
		Context.RoleId = Organization ? Proxy->GetPositionRole(SelectedPositionIndex) : FRoleId();
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

	URtsActions::ExecuteAction(ActionType, Context);
}

void UPlayerCharacterViewModel::BeginDestroy()
{
	if (Player)
	{
		Player->OnControlledCharacterIdChanged.RemoveAll(this);
	}
	Super::BeginDestroy();
}
