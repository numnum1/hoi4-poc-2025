#include "RtsResources.h"

#include "Banking.h"
#include "Characters.h"

#define LOCTEXT_NAMESPACE "RtsResources"

namespace
{
	/**
	 * Every character currently holding a position at InOrganizationId — the organization's
	 * supporters. Linear over all characters: CLAUDE.md sanctions exactly this "Sammel-Getter ohne
	 * Rückwärtsindex" for the UI layer, the same shape URtsActions::HoldsClaimedPosition uses.
	 */
	int32 CountSupporters(const UCharacters* InCharacters, const FOrganizationId InOrganizationId)
	{
		if (InCharacters == nullptr || !InOrganizationId.IsValid())
		{
			return 0;
		}

		int32 Count = 0;
		for (int32 Index = 0; Index < InCharacters->Num(); ++Index)
		{
			const FCharacterId Id(Index);
			if (!InCharacters->IsAlive(Id))
			{
				continue;
			}

			for (const FPosition& Position : InCharacters->GetPositions(Id))
			{
				if (Position.OrganizationId == InOrganizationId)
				{
					++Count;
					break;
				}
			}
		}
		return Count;
	}
}

TArray<EResourceType> URtsResources::GetResourceTypes(const EResourceOwnerType InOwnerType)
{
	switch (InOwnerType)
	{
	case EResourceOwnerType::ROT_Character:
		return { EResourceType::RT_Money, EResourceType::RT_Crystals };
	case EResourceOwnerType::ROT_Organization:
		// Manpower is an organization's supporters — a character has none of their own.
		return { EResourceType::RT_Money, EResourceType::RT_Manpower, EResourceType::RT_Crystals };
	case EResourceOwnerType::ROT_None:
	default:
		return {};
	}
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
			const FBankAccountId AccountId = InContext.OwnerType == EResourceOwnerType::ROT_Character
				? InContext.Banking->FindAccountForCharacter(InContext.OwnerCharacterId)
				: InContext.Banking->FindAccountForOrganization(InContext.OwnerOrganizationId);
			return InContext.Banking->GetBalance(AccountId);
		}
		// No credit/interest model exists yet — see Banking module's FBankTag comment for where a
		// bank's credit policy eventually attaches.
		case EResourceDetail::RD_Credit:      return 0; // future: the account's credit line
		case EResourceDetail::RD_Outstanding: return 0; // future: sum of unpaid dues
		default: return 0;
		}

	case EResourceType::RT_Manpower:
	{
		// A character has no supporters of their own — only an organization's positions count.
		const int32 Total = InContext.OwnerType == EResourceOwnerType::ROT_Organization
			? CountSupporters(InContext.Characters, InContext.OwnerOrganizationId)
			: 0;

		switch (InDetail)
		{
		case EResourceDetail::RD_Total:
			return Total;
		case EResourceDetail::RD_Used:
			// No task/assignment concept exists yet, so nobody is counted as busy.
			return 0;
		case EResourceDetail::RD_Free:
			// Total - Used; Used is 0 today, so every supporter reads as free until tasks exist.
			return Total;
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
