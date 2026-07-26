#include "ActionViewModel.h"

#include "CharacterProxy.h"
#include "OrganizationProxy.h"
#include "Roles.h"
#include "RtsProxySubsystem.h"

#define LOCTEXT_NAMESPACE "ActionViewModel"

void UActionViewModel::SetContext(const FRtsActionContext& InContext)
{
	Context = InContext;

	const FText NewBlockedReason = GetGatedBlockedReason(Context);
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, GetDisplayNameFor(Context));
	UE_MVVM_SET_PROPERTY_VALUE(bIsEnabled, NewBlockedReason.IsEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(BlockedReason, NewBlockedReason);
}

bool UActionViewModel::Execute()
{
	if (!GetGatedBlockedReason(Context).IsEmpty())
	{
		return false;
	}

	ExecuteAction(Context);
	return true;
}

bool UActionViewModel::HoldsClaimedPosition(const FRtsActionContext& InContext) const
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

FText UActionViewModel::GetGatedBlockedReason(const FRtsActionContext& InContext) const
{
	if (InContext.Actor == nullptr || !InContext.Actor->IsAlive())
	{
		return LOCTEXT("NoActor", "No acting character.");
	}

	if (InContext.Scope != GetScope())
	{
		return LOCTEXT("WrongScope", "Not available in this role.");
	}

	if (GetScope() != ERtsActionScope::RAS_Position)
	{
		// Self-scoped: there is no position and no role to check against yet.
		return GetActionBlockedReason(InContext);
	}

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

	if (!Roles->HasPermissions(InContext.RoleId, GetRequiredPermissions()))
	{
		return LOCTEXT("MissingPermission", "Your role does not grant this.");
	}

	return GetActionBlockedReason(InContext);
}

#undef LOCTEXT_NAMESPACE
