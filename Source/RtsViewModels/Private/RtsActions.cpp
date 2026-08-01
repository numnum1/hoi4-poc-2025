#include "RtsActions.h"

#include "Characters.h"
#include "OrganizationInfo.h"
#include "Organizations.h"
#include "Roles.h"

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
		if (InContext.Characters == nullptr)
		{
			return false;
		}

		for (const FPosition& Position : InContext.Characters->GetPositions(InContext.ActorId))
		{
			if (Position.OrganizationId == InContext.OrganizationId && Position.RoleId == InContext.RoleId)
			{
				return true;
			}
		}
		return false;
	}
}

FText URtsActions::GetBlockedReason(const EAction InAction, const FRtsActionContext& InContext)
{
	if (InContext.Characters == nullptr || !InContext.ActorId.IsValid() || !InContext.Characters->IsAlive(InContext.ActorId))
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

		if (InContext.Roles == nullptr)
		{
			return LOCTEXT("NoRoleLibrary", "Role library unavailable.");
		}

		if (!InContext.Roles->HasPermissions(InContext.RoleId, GetRequiredPermissions(InAction)))
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

FText URtsActions::GetInvalidInputReason(const EAction InAction, const FRtsActionContext& InContext,
                                         const FRtsActionInput& InInput)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		if (InInput.OrganizationName.IsEmptyOrWhitespace())
		{
			return LOCTEXT("EmptyOrganizationName", "Enter a name for the organization.");
		}
		return FText::GetEmpty();
	}
	return FText::GetEmpty();
}

void URtsActions::ExecuteAction(const EAction InAction, const FRtsActionContext& InContext,
                                const FRtsActionInput& InInput)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
	{
		if (InContext.Organizations == nullptr || InContext.Characters == nullptr)
		{
			return;
		}

		FOrganizationInfo Info;
		Info.OrganizationName = InInput.OrganizationName;

		const FOrganizationId NewOrganizationId = InContext.Organizations->Add(Info);

		// An unauthored role library is legal — the founder still gets their position, just without
		// a role to hold. See URoles::GetLeadershipRole.
		const FRoleId LeadershipRole = InContext.Roles ? InContext.Roles->GetLeadershipRole() : FRoleId();

		InContext.Characters->AddPosition(InContext.ActorId, NewOrganizationId, LeadershipRole);
		return;
	}
	}
}

#undef LOCTEXT_NAMESPACE
