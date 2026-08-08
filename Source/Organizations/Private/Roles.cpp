#include "Roles.h"

URoles::URoles()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void URoles::Setup(TArray<FRoleInfo> InRoles)
{
	Values = MoveTemp(InRoles);
}

FRoleInfo URoles::GetRoleInfo(const FRoleId InRoleId) const
{
	return Values.IsValidIndex(InRoleId.Value) ? Values[InRoleId.Value] : FRoleInfo();
}

FText URoles::GetRoleName(const FRoleId InRoleId) const
{
	return Values.IsValidIndex(InRoleId.Value) ? Values[InRoleId.Value].RoleName : FText::GetEmpty();
}

bool URoles::HasPermissions(const FRoleId InRoleId, const int32 InRequiredPermissions) const
{
	if (!Values.IsValidIndex(InRoleId.Value))
	{
		return false;
	}
	return (Values[InRoleId.Value].Permissions & InRequiredPermissions) == InRequiredPermissions;
}

FRoleId URoles::GetLeadershipRole() const
{
	for (int32 Index = 0; Index < Values.Num(); ++Index)
	{
		if (Values[Index].bIsLeadership)
		{
			return FRoleId(Index);
		}
	}
	return FRoleId();
}

FRoleId URoles::GetMembershipRole() const
{
	for (int32 Index = 0; Index < Values.Num(); ++Index)
	{
		if (Values[Index].bIsDefaultMembership)
		{
			return FRoleId(Index);
		}
	}
	return FRoleId();
}
