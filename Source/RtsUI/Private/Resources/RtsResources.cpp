#include "Resources/RtsResources.h"

#include "Banking.h"
#include "Characters.h"
#include "Presence/RtsPresence.h"

#define LOCTEXT_NAMESPACE "RtsResources"

TArray<EResourceType> URtsResources::FindResourcesToDisplay(const FResourceOwner& InOwner)
{
	switch (InOwner.OwnerType)
	{
	case EResourceOwnerType::ROT_Character:
		return FindResourcesToDisplayForCharacter(InOwner.CharacterId);
	case EResourceOwnerType::ROT_Organization:
		return FindResourcesToDisplayForOrganization(InOwner.OrganizationId);
	case EResourceOwnerType::ROT_None:
	default:
		// Not "show nothing because everything is zero" — there is no legal person to show a panel
		// for at all, which is the one case that legitimately has no rows.
		return {};
	}
}

TArray<EResourceType> URtsResources::FindResourcesToDisplayForCharacter(const FCharacterId InCharacterId)
{
	// The id is unused today because every character shows the same two rows. It stays in the
	// signature because this is the seam where a per-character difference (a race that trades in
	// something else, say) would be answered, and callers already have the id in hand.
	//
	// Manpower is absent by kind, not by amount: it means an organization's supporters, and a
	// character has none of their own — it would be a permanently meaningless row, not a zero one.
	return { EResourceType::RT_Money, EResourceType::RT_Crystals };
}

TArray<EResourceType> URtsResources::FindResourcesToDisplayForOrganization(const FOrganizationId InOrganizationId)
{
	return { EResourceType::RT_Money, EResourceType::RT_Manpower, EResourceType::RT_Crystals };
}

FText URtsResources::GetDisplayName(const EResourceType InResource)
{
	switch (InResource)
	{
	case EResourceType::RT_Money:
		return LOCTEXT("Money", "Money");
	case EResourceType::RT_Manpower:
		return LOCTEXT("Manpower", "Manpower");
	case EResourceType::RT_Crystals:
		return LOCTEXT("Crystals", "Crystals");
	}
	return FText::GetEmpty();
}

FSlateBrush URtsResources::GetIcon(const EResourceType InResource)
{
	switch (InResource)
	{
	case EResourceType::RT_Money:
	case EResourceType::RT_Manpower:
	case EResourceType::RT_Crystals:
		return FSlateNoResource();
	}
	return FSlateNoResource();
}

TArray<EResourceDetail> URtsResources::GetDetails(const EResourceType InResource)
{
	switch (InResource)
	{
	case EResourceType::RT_Money:
		return { EResourceDetail::RD_Total, EResourceDetail::RD_Credit, EResourceDetail::RD_Outstanding };
	case EResourceType::RT_Manpower:
		return { EResourceDetail::RD_Total, EResourceDetail::RD_Used, EResourceDetail::RD_Free };
	case EResourceType::RT_Crystals:
		return { EResourceDetail::RD_Total };
	}
	return {};
}

FText URtsResources::GetDetailName(const EResourceType InResource, const EResourceDetail InDetail)
{
	// Switches on the resource first so each branch only names the details GetDetails actually
	// offers for it — a shared switch on InDetail alone could not tell "Money's Total" from
	// "Manpower's Total" if the two ever need to read differently.
	switch (InResource)
	{
	case EResourceType::RT_Money:
		switch (InDetail)
		{
		case EResourceDetail::RD_Total:       return LOCTEXT("MoneyTotal", "Total");
		case EResourceDetail::RD_Credit:      return LOCTEXT("MoneyCredit", "Credit");
		case EResourceDetail::RD_Outstanding: return LOCTEXT("MoneyOutstanding", "Outstanding Payments");
		default: break;
		}
		break;

	case EResourceType::RT_Manpower:
		switch (InDetail)
		{
		case EResourceDetail::RD_Total: return LOCTEXT("ManpowerTotal", "Total");
		case EResourceDetail::RD_Used:  return LOCTEXT("ManpowerUsed", "Used");
		case EResourceDetail::RD_Free:  return LOCTEXT("ManpowerFree", "Free");
		default: break;
		}
		break;

	case EResourceType::RT_Crystals:
		switch (InDetail)
		{
		case EResourceDetail::RD_Total: return LOCTEXT("CrystalsTotal", "Total");
		default: break;
		}
		break;
	}
	return FText::GetEmpty();
}

int32 URtsResources::GetAmount(const EResourceType InResource, const EResourceDetail InDetail, const FRtsResourceContext& InContext)
{
	switch (InResource)
	{
	case EResourceType::RT_Money:
		switch (InDetail)
		{
		case EResourceDetail::RD_Total:
		{
			if (InContext.Banking == nullptr)
			{
				return 0;
			}

			// An owner with no open account yet is not a bug — GetBalance(an invalid id) is 0.
			if (InContext.Owner.OwnerType == EResourceOwnerType::ROT_Character)
			{
				int32 Total = 0;
				for (const FBankAccountId AccountId : InContext.Banking->GetAccountsForCharacter(InContext.Owner.CharacterId))
				{
					Total += InContext.Banking->GetBalance(AccountId);
				}
				return Total;
			}

			const FBankAccountId AccountId = InContext.Banking->FindAccountForOrganization(InContext.Owner.OrganizationId);
			return InContext.Banking->GetBalance(AccountId);
		}
		// No credit/interest model exists yet — see Banking module's FBankTag comment for where a
		// bank's credit policy eventually attaches.
		case EResourceDetail::RD_Credit:      return 0;
		case EResourceDetail::RD_Outstanding: return 0;
		default: return 0;
		}

	case EResourceType::RT_Manpower:
	{
		// A character has no seats of their own — only an organization's departments have room to
		// fill. Total is the room itself (URtsPresence::GetCapacity), Used is how much of it is
		// filled (URtsPresence::CountMembers) — recruiting is what moves Used toward Total, and
		// building more department space is what moves Total itself.
		if (InContext.Owner.OwnerType != EResourceOwnerType::ROT_Organization)
		{
			return 0;
		}

		const int32 Capacity = URtsPresence::GetCapacity(InContext.Owner.OrganizationId, InContext.Departments);
		const int32 Used = URtsPresence::CountMembers(InContext.Owner.OrganizationId, InContext.Characters);

		switch (InDetail)
		{
		case EResourceDetail::RD_Total:
			return Capacity;
		case EResourceDetail::RD_Used:
			return Used;
		case EResourceDetail::RD_Free:
			return FMath::Max(Capacity - Used, 0);
		default:
			return 0;
		}
	}

	case EResourceType::RT_Crystals:
		// No stockpile fragment exists yet — see ProvinceFragments.h's "Population, resources and
		// stability belong next to this one" note for where an owner's stock eventually lives.
		return 0;
	}
	return 0;
}

FText URtsResources::FormatAmount(const EResourceType InResource, const int32 InAmount)
{
	switch (InResource)
	{
	case EResourceType::RT_Money:
		// No currency has been authored yet; once one exists, this is where its symbol goes.
		return FText::AsNumber(InAmount);
	case EResourceType::RT_Manpower:
	case EResourceType::RT_Crystals:
	default:
		return FText::AsNumber(InAmount);
	}
}

#undef LOCTEXT_NAMESPACE
