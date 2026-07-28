#include "CharacterActionsViewModel.h"

#include "ActionViewModel.h"
#include "CharacterProxy.h"
#include "OrganizationProxy.h"
#include "Organizations.h"
#include "Roles.h"
#include "RtsActions.h"
#include "RtsProxySubsystem.h"

#define LOCTEXT_NAMESPACE "CharacterActionsViewModel"

void UCharacterActionsViewModel::Initialize(const FCharacterId InCharacterId)
{
	URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this);
	UCharacterProxy* NewProxy = ProxySubsystem ? ProxySubsystem->GetCharacter(InCharacterId) : nullptr;

	if (Proxy == NewProxy && CharacterId == InCharacterId)
	{
		return;
	}

	Unbind();
	Proxy = NewProxy;
	CharacterId = InCharacterId;
	Organizations = ProxySubsystem ? ProxySubsystem->GetOrganizationsContainer() : nullptr;

	Context = FRtsActionContext();
	Context.Actor = Proxy;

	if (Proxy)
	{
		Proxy->OnChanged.AddDynamic(this, &UCharacterActionsViewModel::HandleProxyChanged);
	}
	if (Organizations)
	{
		Organizations->OnOrganizationAdded.AddDynamic(this, &UCharacterActionsViewModel::HandleOrganizationAdded);
	}

	Refresh();
}

void UCharacterActionsViewModel::SetScopeSelf()
{
	Context.Scope = ERtsActionScope::RAS_Self;
	Context.OrganizationId = FOrganizationId();
	Context.RoleId = FRoleId();
	Refresh();
}

void UCharacterActionsViewModel::SetScopePosition(const int32 InPositionIndex)
{
	Context.Scope = ERtsActionScope::RAS_Position;

	// Organization and role are read from the same position index, so a context can never claim a
	// role at an organization the character does not hold it at.
	const UOrganizationProxy* Organization = Proxy ? Proxy->GetOrganization(InPositionIndex) : nullptr;
	Context.OrganizationId = Organization ? Organization->GetId() : FOrganizationId();
	Context.RoleId = Organization ? Proxy->GetPositionRole(InPositionIndex) : FRoleId();

	Refresh();
}

void UCharacterActionsViewModel::Refresh_Implementation()
{
	TArray<EAction> OfferedActions;
	if (Context.Scope == ERtsActionScope::RAS_Self)
	{
		OfferedActions = URtsActions::GetActionsForSelf();
	}
	else
	{
		FPosition RequestedPosition;
		RequestedPosition.OrganizationId = Context.OrganizationId;
		RequestedPosition.RoleId = Context.RoleId;
		OfferedActions = URtsActions::GetActionsForPosition(RequestedPosition);
	}

	TArray<TObjectPtr<UActionViewModel>> NewActions;
	NewActions.Reserve(OfferedActions.Num());

	for (const EAction ActionType : OfferedActions)
	{
		TObjectPtr<UActionViewModel>& Entry = EntryCache.FindOrAdd(ActionType);
		if (Entry == nullptr)
		{
			Entry = NewObject<UActionViewModel>(this);
			Entry->OnClicked.AddDynamic(this, &UCharacterActionsViewModel::HandleActionClicked);
		}

		Entry->SetContext(ActionType, Context);
		const FText BlockedReason = GetGatedBlockedReason(Entry);
		Entry->SetEnabled(BlockedReason.IsEmpty(), BlockedReason);
		NewActions.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Actions, NewActions);
	UE_MVVM_SET_PROPERTY_VALUE(ActionCount, NewActions.Num());
}

void UCharacterActionsViewModel::HandleProxyChanged()
{
	Refresh();
}

void UCharacterActionsViewModel::HandleOrganizationAdded()
{
	Refresh();
}

void UCharacterActionsViewModel::HandleActionClicked(UActionViewModel* ClickedAction)
{
	if (ClickedAction == nullptr || !GetGatedBlockedReason(ClickedAction).IsEmpty())
	{
		return;
	}

	URtsActions::ExecuteAction(ClickedAction->GetActionType(), Context);
}

bool UCharacterActionsViewModel::HoldsClaimedPosition() const
{
	const int32 Count = Proxy->GetOrganizationCount();
	for (int32 Index = 0; Index < Count; ++Index)
	{
		const UOrganizationProxy* Organization = Proxy->GetOrganization(Index);
		if (Organization != nullptr
			&& Organization->GetId() == Context.OrganizationId
			&& Proxy->GetPositionRole(Index) == Context.RoleId)
		{
			return true;
		}
	}
	return false;
}

FText UCharacterActionsViewModel::GetGatedBlockedReason(const UActionViewModel* Action) const
{
	if (Proxy == nullptr || !Proxy->IsAlive())
	{
		return LOCTEXT("NoActor", "No acting character.");
	}

	const EAction ActionType = Action->GetActionType();

	if (Context.Scope != URtsActions::GetScope(ActionType))
	{
		return LOCTEXT("WrongScope", "Not available in this role.");
	}

	if (URtsActions::GetScope(ActionType) != ERtsActionScope::RAS_Position)
	{
		// Self-scoped: there is no position and no role to check against yet.
		return URtsActions::GetBlockedReason(ActionType, Context);
	}

	if (!HoldsClaimedPosition())
	{
		return LOCTEXT("PositionNotHeld", "You do not hold that position.");
	}

	URtsProxySubsystem* Proxies = Proxy->GetProxies();
	const URoles* Roles = Proxies ? Proxies->GetRolesContainer() : nullptr;
	if (Roles == nullptr)
	{
		return LOCTEXT("NoRoleLibrary", "Role library unavailable.");
	}

	if (!Roles->HasPermissions(Context.RoleId, URtsActions::GetRequiredPermissions(ActionType)))
	{
		return LOCTEXT("MissingPermission", "Your role does not grant this.");
	}

	return URtsActions::GetBlockedReason(ActionType, Context);
}

void UCharacterActionsViewModel::Unbind()
{
	if (Proxy)
	{
		Proxy->OnChanged.RemoveAll(this);
	}
	if (Organizations)
	{
		Organizations->OnOrganizationAdded.RemoveAll(this);
	}
}

void UCharacterActionsViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}

#undef LOCTEXT_NAMESPACE
