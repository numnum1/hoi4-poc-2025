#include "RtsActions.h"

#include "Banking.h"
#include "BuildingType.h"
#include "Buildings.h"
#include "Characters.h"
#include "Departments.h"
#include "OrganizationInfo.h"
#include "Organizations.h"
#include "Roles.h"
#include "RtsPresence.h"

#define LOCTEXT_NAMESPACE "RtsActions"

TArray<EAction> URtsActions::GetActionsForSelf()
{
	return { EAction::EA_FoundOrganization, EAction::EA_JoinOrganization };
}

TArray<EAction> URtsActions::GetActionsForPosition(const FPosition InPosition)
{
	// Every position can be left; recruiting is gated by permission inside GetBlockedReason.
	return { EAction::EA_LeaveOrganization, EAction::EA_RecruitCharacter };
}

ERtsActionScope URtsActions::GetScope(const EAction InAction)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		return ERtsActionScope::RAS_Self;
	case EAction::EA_JoinOrganization:
		return ERtsActionScope::RAS_Self;
	case EAction::EA_LeaveOrganization:
		return ERtsActionScope::RAS_Position;
	case EAction::EA_RecruitCharacter:
		return ERtsActionScope::RAS_Position;
	}
	return ERtsActionScope::RAS_Self;
}

FText URtsActions::GetDisplayName(const EAction InAction)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
		return LOCTEXT("FoundOrganization", "Found Organization");
	case EAction::EA_JoinOrganization:
		return LOCTEXT("JoinOrganization", "Join Organization");
	case EAction::EA_LeaveOrganization:
		return LOCTEXT("LeaveOrganization", "Leave Organization");
	case EAction::EA_RecruitCharacter:
		return LOCTEXT("RecruitCharacter", "Recruit Character");
	}
	return FText::GetEmpty();
}

FSlateBrush URtsActions::GetIcon(const EAction InAction)
{
	switch (InAction)
	{
	case EAction::EA_FoundOrganization:
	case EAction::EA_JoinOrganization:
	case EAction::EA_LeaveOrganization:
	case EAction::EA_RecruitCharacter:
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
	case EAction::EA_JoinOrganization:
		return 0;
	case EAction::EA_LeaveOrganization:
		// Leaving needs no permission — every position can walk away from itself.
		return 0;
	case EAction::EA_RecruitCharacter:
		return static_cast<int32>(ERolePermission::RP_Recruit);
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

	/** Does the character hold any position (any role) at InOrganizationId — regardless of which
	 * role, unlike HoldsClaimedPosition above, which pins down the exact (organization, role) pair
	 * a RAS_Position context claims. Used where the question is only "is this a member". */
	bool HoldsAnyPositionAt(const UCharacters* InCharacters, const FCharacterId InCharacterId, const FOrganizationId InOrganizationId)
	{
		if (InCharacters == nullptr)
		{
			return false;
		}

		for (const FPosition& Position : InCharacters->GetPositions(InCharacterId))
		{
			if (Position.OrganizationId == InOrganizationId)
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

	case EAction::EA_JoinOrganization:
		return FText::GetEmpty();

	case EAction::EA_LeaveOrganization:
		// The owner cannot walk away and leave the organization headless — see
		// FOrganizationOwnerFragment. There is no hand-off/dissolve flow yet, so this is a hard stop.
		if (InContext.Organizations && InContext.Organizations->IsOwnedBy(InContext.OrganizationId, InContext.ActorId))
		{
			return LOCTEXT("OwnerCannotLeave", "You founded this organization and cannot leave it.");
		}
		return FText::GetEmpty();

	case EAction::EA_RecruitCharacter:
		// The shared RAS_Position gate above already checked RP_Recruit; who to recruit is an
		// input question, answered in GetInvalidInputReason.
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

		if (InInput.HeadquartersType == nullptr || !InInput.HeadquartersType->IsHeadquarters())
		{
			return LOCTEXT("NotAHeadquarters", "Pick a headquarters to found the organization in.");
		}

		// The root department has to do something, and what it does comes from the building. A
		// headquarters type with no authored function cannot seat one — an authoring mistake the
		// player would otherwise only notice as an organization that does nothing.
		EBuildingFunction UnusedFunction;
		if (!InInput.HeadquartersType->GetPrimaryFunction(UnusedFunction))
		{
			return LOCTEXT("NoBuildingFunction", "This building offers no function to seat a department in.");
		}

		if (!InInput.HeadquartersProvinceId.IsValid())
		{
			return LOCTEXT("NoProvince", "Click a province on the map to place the headquarters.");
		}

		const int32 Cost = InInput.HeadquartersType->GetBuildCost();
		if (GetCharacterFunds(InContext.Banking, InContext.ActorId) < Cost)
		{
			return FText::Format(LOCTEXT("CannotAffordHeadquarters", "You cannot afford this: it costs {0}."),
				FText::AsNumber(Cost));
		}

		return FText::GetEmpty();
	}

	case EAction::EA_JoinOrganization:
	{
		if (InContext.Organizations == nullptr || !InContext.Organizations->IsAlive(InInput.TargetOrganizationId))
		{
			return LOCTEXT("NoJoinTarget", "Pick an organization to join.");
		}

		if (HoldsAnyPositionAt(InContext.Characters, InContext.ActorId, InInput.TargetOrganizationId))
		{
			return LOCTEXT("AlreadyMember", "You are already a member of this organization.");
		}

		// Presence is re-derived from where the character is standing right now, not from
		// whatever the province screen showed when the player opened the modal.
		const FProvinceId CurrentProvinceId = InContext.Characters ? InContext.Characters->GetCurrentProvince(InContext.ActorId) : FProvinceId();
		const TArray<FOrganizationId> PresentHere = URtsPresence::CollectOrganizationsIn(CurrentProvinceId, InContext.Departments, InContext.Buildings);
		if (!PresentHere.Contains(InInput.TargetOrganizationId))
		{
			return LOCTEXT("NotPresentHere", "This organization has no seat where you are standing.");
		}

		const int32 Capacity = URtsPresence::GetCapacity(InInput.TargetOrganizationId, InContext.Departments);
		const int32 Members = URtsPresence::CountMembers(InInput.TargetOrganizationId, InContext.Characters);
		if (Members >= Capacity)
		{
			return LOCTEXT("OrganizationFull", "This organization has no room for another member.");
		}

		return FText::GetEmpty();
	}

	case EAction::EA_RecruitCharacter:
	{
		if (InContext.Characters == nullptr || !InContext.Characters->IsAlive(InInput.TargetCharacterId))
		{
			return LOCTEXT("NoRecruitTarget", "Pick someone to recruit.");
		}

		if (HoldsAnyPositionAt(InContext.Characters, InInput.TargetCharacterId, InContext.OrganizationId))
		{
			return LOCTEXT("AlreadyRecruited", "They already hold a position here.");
		}

		// Recruiting happens at the headquarters specifically, not anywhere the organization has a
		// seat — same reasoning as founding: the root department is where the organization itself
		// is, a branch office is a different question this round does not answer yet.
		const FProvinceId SeatProvinceId = URtsPresence::FindSeatProvince(InContext.OrganizationId, InContext.Organizations, InContext.Departments, InContext.Buildings);
		if (!SeatProvinceId.IsValid() || InContext.Characters->GetCurrentProvince(InInput.TargetCharacterId) != SeatProvinceId)
		{
			return LOCTEXT("NotAtSeat", "They are not where this organization's headquarters is seated.");
		}

		const int32 Capacity = URtsPresence::GetCapacity(InContext.OrganizationId, InContext.Departments);
		const int32 Members = URtsPresence::CountMembers(InContext.OrganizationId, InContext.Characters);
		if (Members >= Capacity)
		{
			return LOCTEXT("NoRoomToRecruit", "This organization has no room for another member.");
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
		if (InContext.Organizations == nullptr || InContext.Characters == nullptr
			|| InContext.Buildings == nullptr || InContext.Departments == nullptr)
		{
			return;
		}

		EBuildingFunction RootFunction;
		if (InInput.HeadquartersType == nullptr || !InInput.HeadquartersType->GetPrimaryFunction(RootFunction))
		{
			return;
		}

		// Paid first, and all of it or none: nothing below is created unless the founder could
		// actually cover the headquarters.
		if (!PayFromCharacter(InContext.Banking, InContext.ActorId, InInput.HeadquartersType->GetBuildCost()))
		{
			return;
		}

		// The founder puts the building up and keeps it. The organization never owns buildings — it
		// owns a department, and that department is only *seated* here.
		const FBuildingId HeadquartersId = InContext.Buildings->Add(
			InInput.HeadquartersType, InInput.HeadquartersProvinceId, InContext.ActorId);

		FOrganizationInfo Info;
		Info.OrganizationName = InInput.OrganizationName;

		// Founding is what makes a character an owner: the organization records the founder, and the
		// founder gets the position that lets them act for it. Two facts, because they answer two
		// different questions — see FOrganizationOwnerFragment.
		const FOrganizationId NewOrganizationId = InContext.Organizations->Add(Info, InContext.ActorId);

		// The organization banks where its founder already does — "you bank where you bank" instead
		// of inventing a bank picker. No account of the founder's own means no account for the
		// organization either; that is legal, GetBalance(an invalid id) is 0, same as an unopened one.
		if (InContext.Banking)
		{
			const TArray<FBankAccountId> FounderAccounts = InContext.Banking->GetAccountsForCharacter(InContext.ActorId);
			if (FounderAccounts.Num() > 0)
			{
				const FBankId FounderBankId = InContext.Banking->GetBank(FounderAccounts[0]);
				InContext.Banking->OpenAccountForOrganization(FounderBankId, NewOrganizationId);
			}
		}

		// The root department takes the whole building for its function: it is the first tenant, so
		// there is nothing to share with yet, and a founder should not have to allocate space in a
		// building they just put up for exactly this.
		const FDepartmentId RootDepartmentId = InContext.Departments->Add(
			NewOrganizationId, /*InParentId=*/FDepartmentId(), HeadquartersId, RootFunction,
			InInput.HeadquartersType->GetSpaceFor(RootFunction));
		InContext.Organizations->SetRootDepartment(NewOrganizationId, RootDepartmentId);

		// An unauthored role library is legal — the founder still gets their position, just without
		// a role to hold. See URoles::GetLeadershipRole.
		const FRoleId LeadershipRole = InContext.Roles ? InContext.Roles->GetLeadershipRole() : FRoleId();

		InContext.Characters->AddPosition(InContext.ActorId, NewOrganizationId, LeadershipRole);
		return;
	}

	case EAction::EA_JoinOrganization:
	{
		if (InContext.Characters == nullptr)
		{
			return;
		}

		// An unauthored role library is legal — the member still gets their position, just without
		// a role to hold. See URoles::GetLeadershipRole for the same shape at founding.
		const FRoleId MembershipRole = InContext.Roles ? InContext.Roles->GetMembershipRole() : FRoleId();
		InContext.Characters->AddPosition(InContext.ActorId, InInput.TargetOrganizationId, MembershipRole);
		return;
	}

	case EAction::EA_LeaveOrganization:
	{
		if (InContext.Characters == nullptr)
		{
			return;
		}

		InContext.Characters->RemovePosition(InContext.ActorId, InContext.OrganizationId);
		return;
	}

	case EAction::EA_RecruitCharacter:
	{
		if (InContext.Characters == nullptr)
		{
			return;
		}

		// Same unauthored-library legality as founding/joining.
		const FRoleId MembershipRole = InContext.Roles ? InContext.Roles->GetMembershipRole() : FRoleId();
		InContext.Characters->AddPosition(InInput.TargetCharacterId, InContext.OrganizationId, MembershipRole);
		return;
	}
	}
}

#undef LOCTEXT_NAMESPACE
