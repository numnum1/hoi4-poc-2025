#include "RtsActions.h"

#include "CharacterProxy.h"
#include "Characters.h"
#include "OrganizationInfo.h"
#include "OrganizationProxy.h"
#include "Organizations.h"
#include "Roles.h"
#include "RtsProxySubsystem.h"

#define LOCTEXT_NAMESPACE "RtsActions"

TArray<EAction> URtsActions::GetActionsForSelf()
{
	return { EAction::EA_FoundOrganization };
}

TArray<EAction> URtsActions::GetActionsForPosition(const FPosition InPosition)
{
	// No position-scoped action exists yet.
	return {};
}

ERtsActionScope URtsActions::GetScope(const EAction InAction)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		return ERtsActionScope::RAS_Self;
	}
	return ERtsActionScope::RAS_Self;
}

FText URtsActions::GetDisplayName(const EAction InAction)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		return LOCTEXT("FoundOrganization", "Found Organization");
	}
	return FText::GetEmpty();
}

FSlateBrush URtsActions::GetIcon(const EAction InAction)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		return FSlateNoResource();
	}
	return FSlateNoResource();
}

int32 URtsActions::GetRequiredPermissions(const EAction InAction)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		return 0;
	}
	return 0;
}

namespace
{
	/** Does the acting character really hold the (organization, role) pair the context claims? */
	bool HoldsClaimedPosition(const FRtsActionContext& InContext)
	{
		const int32 Count = InContext.Actor->GetOrganizationCount();
		for (int32 Index = 0; Index < Count; ++Index)
		{
			const UOrganizationProxy* Organization = InContext.Actor->GetOrganization(Index);
			if (Organization != nullptr
				&& Organization->GetId() == InContext.OrganizationId
				&& InContext.Actor->GetPositionRole(Index) == InContext.RoleId)
			{
				return true;
			}
		}
		return false;
	}
}

FText URtsActions::GetBlockedReason(const EAction InAction, const FRtsActionContext& InContext)
{
	if (InContext.Actor == nullptr || !InContext.Actor->IsAlive())
	{
		return LOCTEXT("NoActor", "No acting character.");
	}

	if (InContext.Scope != GetScope(InAction))
	{
		return LOCTEXT("WrongScope", "Not available in this role.");
	}

	if (GetScope(InAction) == ERtsActionScope::RAS_Position)
	{
		if (!HoldsClaimedPosition(InContext))
		{
			return LOCTEXT("PositionNotHeld", "You do not hold that position.");
		}

		URtsProxySubsystem* Proxies = InContext.Actor->GetProxies();
		const URoles* Roles = Proxies ? Proxies->GetRolesContainer() : nullptr;
		if (Roles == nullptr)
		{
			return LOCTEXT("NoRoleLibrary", "Role library unavailable.");
		}

		if (!Roles->HasPermissions(InContext.RoleId, GetRequiredPermissions(InAction)))
		{
			return LOCTEXT("MissingPermission", "Your role does not grant this.");
		}
	}

	// Action-specific checks on top of the shared gate.
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		return FText::GetEmpty();
	}
	return FText::GetEmpty();
}

void URtsActions::ExecuteAction(const EAction InAction, const FRtsActionContext& InContext)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
	{
		URtsProxySubsystem* Proxies = InContext.Actor->GetProxies();
		if (Proxies == nullptr)
		{
			return;
		}

		UOrganizations* Organizations = Proxies->GetOrganizationsContainer();
		UCharacters* Characters = Proxies->GetCharactersContainer();
		if (Organizations == nullptr || Characters == nullptr)
		{
			return;
		}

		FOrganizationInfo Info;
		Info.OrganizationName = FText::Format(
			LOCTEXT("FoundedOrganizationName", "{0}'s Organization"), InContext.Actor->GetDisplayName());

		const FOrganizationId NewOrganizationId = Organizations->Add(Info);

		// An unauthored role library is legal — the founder still gets their position, just without
		// a role to hold. See URoles::GetLeadershipRole.
		const URoles* Roles = Proxies->GetRolesContainer();
		const FRoleId LeadershipRole = Roles ? Roles->GetLeadershipRole() : FRoleId();

		Characters->AddPosition(InContext.Actor->GetId(), NewOrganizationId, LeadershipRole);
		return;
	}
	}
}

#undef LOCTEXT_NAMESPACE
