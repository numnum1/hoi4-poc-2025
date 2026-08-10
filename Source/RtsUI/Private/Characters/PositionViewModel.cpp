#include "Characters/PositionViewModel.h"

#include "OrganizationInfo.h"
#include "Organizations.h"
#include "RoleInfo.h"
#include "Roles.h"

void UPositionViewModel::SetSource(const FPosition& InPosition, const FCharacterId InHolderId,
                                   UOrganizations* InOrganizations, URoles* InRoles)
{
	const bool bHasOrganization = InOrganizations != nullptr && InPosition.OrganizationId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(OrganizationId, InPosition.OrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationIdValue, InPosition.OrganizationId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationName, bHasOrganization ? InOrganizations->GetOrganizationInfo(InPosition.OrganizationId).OrganizationName : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationLogo, bHasOrganization ? InOrganizations->GetOrganizationInfo(InPosition.OrganizationId).Logo : FSlateBrush());
	UE_MVVM_SET_PROPERTY_VALUE(RoleId, InPosition.RoleId);
	UE_MVVM_SET_PROPERTY_VALUE(RoleIdValue, InPosition.RoleId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(RoleName, InRoles ? InRoles->GetRoleName(InPosition.RoleId) : FText::GetEmpty());
	UE_MVVM_SET_PROPERTY_VALUE(bIsOwner, bHasOrganization && InOrganizations->IsOwnedBy(InPosition.OrganizationId, InHolderId));
}
