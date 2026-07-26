#include "FoundOrganizationActionViewModel.h"

#include "CharacterProxy.h"
#include "Characters.h"
#include "OrganizationInfo.h"
#include "Organizations.h"
#include "Roles.h"
#include "RtsProxySubsystem.h"

#define LOCTEXT_NAMESPACE "FoundOrganizationActionViewModel"

FText UFoundOrganizationActionViewModel::GetDisplayNameFor(const FRtsActionContext& InContext) const
{
	return LOCTEXT("FoundOrganization", "Found Organization");
}

void UFoundOrganizationActionViewModel::ExecuteAction(const FRtsActionContext& InContext) const
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

	// An unauthored role library is legal — the founder still gets their position, just without a
	// role to hold. See URoles::GetLeadershipRole.
	const URoles* Roles = Proxies->GetRolesContainer();
	const FRoleId LeadershipRole = Roles ? Roles->GetLeadershipRole() : FRoleId();

	Characters->AddPosition(InContext.Actor->GetId(), NewOrganizationId, LeadershipRole);
}

#undef LOCTEXT_NAMESPACE
