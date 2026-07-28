#include "PositionViewModel.h"

#include "CharacterProxy.h"
#include "OrganizationProxy.h"
#include "Roles.h"
#include "RtsProxySubsystem.h"

void UPositionViewModel::SetSource(UCharacterProxy* InCharacter, const int32 InPositionIndex)
{
	const UOrganizationProxy* Organization = InCharacter ? InCharacter->GetOrganization(InPositionIndex) : nullptr;
	const FRoleId NewRoleId = InCharacter ? InCharacter->GetPositionRole(InPositionIndex) : FRoleId();

	URtsProxySubsystem* Proxies = InCharacter ? InCharacter->GetProxies() : nullptr;
	const URoles* Roles = Proxies ? Proxies->GetRolesContainer() : nullptr;

	UE_MVVM_SET_PROPERTY_VALUE(OrganizationId, Organization ? Organization->GetId() : FOrganizationId());
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationIdValue, Organization ? Organization->GetId().Value : INDEX_NONE);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, Organization ? Organization->GetDisplayName() : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationLogo, Organization ? Organization->GetLogo() : FSlateBrush());
	UE_MVVM_SET_PROPERTY_VALUE(RoleId, NewRoleId);
	UE_MVVM_SET_PROPERTY_VALUE(RoleIdValue, NewRoleId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(RoleName, Roles ? Roles->GetRoleName(NewRoleId) : FText::GetEmpty());
}
