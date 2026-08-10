#include "Economy/RtsEconomy.h"

#include "Banking.h"
#include "BuildingFunction.h"
#include "Characters.h"
#include "Departments.h"
#include "Organizations.h"
#include "Presence/RtsPresence.h"

namespace
{
	/** What one filled seat of this function earns per day. Balance numbers live in code, not a
	 * DataAsset — same reasoning as URtsActions::GetRequiredPermissions. */
	int32 GetDailyYield(const EBuildingFunction InFunction)
	{
		switch (InFunction)
		{
		case EBuildingFunction::BF_Bureaucracy: return 5;
		case EBuildingFunction::BF_Prayer:      return 6;
		case EBuildingFunction::BF_Production:  return 8;
		case EBuildingFunction::BF_Retail:      return 7;
		}
		return 0;
	}

	constexpr int32 UpkeepPerDepartment = 5;
	constexpr int32 WagePerMember = 2;

	/**
	 * Removes one member to cover a shortfall the organization's account could not pay. Never the
	 * owner — an insolvent organization with no other member just stays insolvent; there is no
	 * dissolve-on-bankruptcy flow yet. The design calls for evicting whoever joined most recently,
	 * but FPosition carries no join order to read that back from, so this picks the highest
	 * character id among the current non-owner members as the closest stand-in — usually the most
	 * recently created or recruited character, though not a strict guarantee.
	 */
	void EvictOneMember(UOrganizations* InOrganizations, UCharacters* InCharacters, const FOrganizationId InOrganizationId)
	{
		if (InOrganizations == nullptr || InCharacters == nullptr)
		{
			return;
		}

		const FCharacterId OwnerId = InOrganizations->GetOwnerCharacter(InOrganizationId);
		const TArray<FCharacterId> Members = URtsPresence::CollectMembers(InOrganizationId, InCharacters);

		for (int32 Index = Members.Num() - 1; Index >= 0; --Index)
		{
			if (Members[Index] != OwnerId)
			{
				InCharacters->RemovePosition(Members[Index], InOrganizationId);
				return;
			}
		}
	}
}

void URtsEconomy::SettleDay(const FRtsEconomyContext& InContext)
{
	if (InContext.Organizations == nullptr || InContext.Departments == nullptr
		|| InContext.Characters == nullptr || InContext.Banking == nullptr)
	{
		return;
	}

	for (int32 Index = 0; Index < InContext.Organizations->Num(); ++Index)
	{
		const FOrganizationId OrganizationId(Index);
		if (!InContext.Organizations->IsAlive(OrganizationId))
		{
			continue;
		}

		const FDepartmentId RootDepartmentId = InContext.Organizations->GetRootDepartment(OrganizationId);
		if (!RootDepartmentId.IsValid())
		{
			// Scenario-authored organizations are founded without a department — nothing here to
			// run an economy off yet.
			continue;
		}

		const int32 Members = URtsPresence::CountMembers(OrganizationId, InContext.Characters);
		const int32 Capacity = URtsPresence::GetCapacity(OrganizationId, InContext.Departments);
		const int32 DepartmentCount = InContext.Departments->CollectDepartmentsOf(OrganizationId).Num();

		const EBuildingFunction RootFunction = InContext.Departments->GetFunction(RootDepartmentId);
		const int32 Income = FMath::Min(Members, Capacity) * GetDailyYield(RootFunction);
		const int32 Upkeep = DepartmentCount * UpkeepPerDepartment;
		const int32 Wages = Members * WagePerMember;
		const int32 Net = Income - Upkeep - Wages;

		if (Net == 0)
		{
			continue;
		}

		const FBankAccountId AccountId = InContext.Banking->FindAccountForOrganization(OrganizationId);

		if (Net > 0)
		{
			InContext.Banking->Deposit(AccountId, Net);
			continue;
		}

		if (!InContext.Banking->Withdraw(AccountId, -Net))
		{
			EvictOneMember(InContext.Organizations, InContext.Characters, OrganizationId);
		}
	}
}
