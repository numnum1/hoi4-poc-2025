#include "RtsActions.h"

#include "Banking.h"
#include "BuildingKinds.h"
#include "Buildings.h"
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

	/** Everything the character has, across every account they hold. The same sum the money row in
	 * the resource panel shows (see URtsResources, RD_Total) — a character's cash is one pot. */
	int32 GetCharacterFunds(const UBanking* InBanking, const FCharacterId InCharacterId)
	{
		if (InBanking == nullptr)
		{
			return 0;
		}

		int32 Total = 0;
		for (const FBankAccountId AccountId : InBanking->GetAccountsForCharacter(InCharacterId))
		{
			Total += InBanking->GetBalance(AccountId);
		}
		return Total;
	}

	/**
	 * Takes InAmount off the character, spreading it over their accounts. Returns false and moves
	 * no money at all if they cannot cover it — the total is checked before the first withdrawal,
	 * so this can never leave a half-paid state behind for the caller to unwind.
	 *
	 * No account picker anywhere: the UI shows one number for a character's money, so paying reads
	 * from the same single pot rather than asking which envelope it comes out of.
	 */
	bool PayFromCharacter(UBanking* InBanking, const FCharacterId InCharacterId, const int32 InAmount)
	{
		if (InAmount <= 0)
		{
			return true;
		}

		if (InBanking == nullptr || GetCharacterFunds(InBanking, InCharacterId) < InAmount)
		{
			return false;
		}

		int32 Remaining = InAmount;
		for (const FBankAccountId AccountId : InBanking->GetAccountsForCharacter(InCharacterId))
		{
			if (Remaining <= 0)
			{
				break;
			}

			const int32 FromThisAccount = FMath::Min(InBanking->GetBalance(AccountId), Remaining);
			if (FromThisAccount > 0 && InBanking->Withdraw(AccountId, FromThisAccount))
			{
				Remaining -= FromThisAccount;
			}
		}

		return Remaining == 0;
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
	{
		// Checked in the order the modal fills them in, so the message always points at the next
		// thing left to do rather than at whatever happens to be last in the struct.
		if (InInput.OrganizationName.IsEmptyOrWhitespace())
		{
			return LOCTEXT("EmptyOrganizationName", "Enter a name for the organization.");
		}

		if (!UBuildingKinds::IsHeadquarters(InInput.HeadquartersKind))
		{
			return LOCTEXT("NotAHeadquarters", "Pick a headquarters to found the organization in.");
		}

		if (!InInput.HeadquartersProvinceId.IsValid())
		{
			return LOCTEXT("NoProvince", "Click a province on the map to place the headquarters.");
		}

		const int32 Cost = UBuildingKinds::GetBuildCost(InInput.HeadquartersKind);
		if (GetCharacterFunds(InContext.Banking, InContext.ActorId) < Cost)
		{
			return FText::Format(LOCTEXT("CannotAffordHeadquarters", "You cannot afford this: it costs {0}."),
				FText::AsNumber(Cost));
		}

		return FText::GetEmpty();
	}
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
		if (InContext.Organizations == nullptr || InContext.Characters == nullptr || InContext.Buildings == nullptr)
		{
			return;
		}

		// Paid first, and all of it or none: nothing below is created unless the founder could
		// actually cover the headquarters.
		if (!PayFromCharacter(InContext.Banking, InContext.ActorId, UBuildingKinds::GetBuildCost(InInput.HeadquartersKind)))
		{
			return;
		}

		FOrganizationInfo Info;
		Info.OrganizationName = InInput.OrganizationName;

		// Founding is what makes a character an owner: the organization records the founder, and the
		// founder gets the position that lets them act for it. Two facts, because they answer two
		// different questions — see FOrganizationOwnerFragment.
		const FOrganizationId NewOrganizationId = InContext.Organizations->Add(Info, InContext.ActorId);

		// The organization has to exist before the building can belong to it, which is why the seat
		// is set afterwards rather than passed into Add.
		const FBuildingId HeadquartersId = InContext.Buildings->AddForOrganization(
			InInput.HeadquartersKind, InInput.HeadquartersProvinceId, NewOrganizationId);
		InContext.Organizations->SetHeadquarters(NewOrganizationId, HeadquartersId);

		// An unauthored role library is legal — the founder still gets their position, just without
		// a role to hold. See URoles::GetLeadershipRole.
		const FRoleId LeadershipRole = InContext.Roles ? InContext.Roles->GetLeadershipRole() : FRoleId();

		InContext.Characters->AddPosition(InContext.ActorId, NewOrganizationId, LeadershipRole);
		return;
	}
	}
}

#undef LOCTEXT_NAMESPACE
