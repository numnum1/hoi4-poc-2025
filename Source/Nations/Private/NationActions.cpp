#include "NationActions.h"

#include "Nations.h"
#include "Organizations.h"

bool UNationActions::RegisterOrganization(UNations* InNations, UOrganizations* InOrganizations,
                                           const FNationId InNationId, const FOrganizationId InOrganizationId)
{
	if (InNations == nullptr || InOrganizations == nullptr || !InNations->IsAlive(InNationId))
	{
		return false;
	}

	if (InOrganizations->GetSettledNations(InOrganizationId).Contains(InNationId))
	{
		return false;
	}

	InOrganizations->SettleNation(InOrganizationId, InNationId);
	return true;
}
