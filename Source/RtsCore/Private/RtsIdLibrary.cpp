#include "RtsIdLibrary.h"

bool URtsIdLibrary::IsValidCharacterId(const FCharacterId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidProvinceId(const FProvinceId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidNationId(const FNationId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidOrganizationId(const FOrganizationId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidBuildingId(const FBuildingId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidRaceId(const FRaceId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidBankId(const FBankId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidBankAccountId(const FBankAccountId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidScheduleId(const FScheduleId& InId)
{
	return InId.Value >= 0;
}

bool URtsIdLibrary::IsValidRoleId(const FRoleId& InId)
{
	return InId.Value >= 0;
}
