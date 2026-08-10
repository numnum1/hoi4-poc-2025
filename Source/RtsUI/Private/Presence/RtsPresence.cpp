#include "Presence/RtsPresence.h"

#include "Buildings.h"
#include "Characters.h"
#include "Departments.h"
#include "Organizations.h"

TArray<FOrganizationId> URtsPresence::CollectOrganizationsIn(const FProvinceId InProvinceId, UDepartments* InDepartments, UBuildings* InBuildings)
{
	TArray<FOrganizationId> Result;
	if (InDepartments == nullptr || InBuildings == nullptr || !InProvinceId.IsValid())
	{
		return Result;
	}

	for (const FDepartmentId DepartmentId : InDepartments->GetAliveDepartments())
	{
		const FBuildingId SeatId = InDepartments->GetSeat(DepartmentId);
		if (InBuildings->GetProvince(SeatId) != InProvinceId)
		{
			continue;
		}

		Result.AddUnique(InDepartments->GetOrganization(DepartmentId));
	}
	return Result;
}

FProvinceId URtsPresence::FindSeatProvince(const FOrganizationId InOrganizationId, UOrganizations* InOrganizations, UDepartments* InDepartments, UBuildings* InBuildings)
{
	if (InOrganizations == nullptr || InDepartments == nullptr || InBuildings == nullptr)
	{
		return FProvinceId();
	}

	const FDepartmentId RootDepartmentId = InOrganizations->GetRootDepartment(InOrganizationId);
	if (!RootDepartmentId.IsValid())
	{
		return FProvinceId();
	}

	return InBuildings->GetProvince(InDepartments->GetSeat(RootDepartmentId));
}

int32 URtsPresence::GetCapacity(const FOrganizationId InOrganizationId, UDepartments* InDepartments)
{
	if (InDepartments == nullptr)
	{
		return 0;
	}

	int32 Total = 0;
	for (const FDepartmentId DepartmentId : InDepartments->CollectDepartmentsOf(InOrganizationId))
	{
		Total += InDepartments->GetSpace(DepartmentId);
	}
	return Total;
}

int32 URtsPresence::CountMembers(const FOrganizationId InOrganizationId, UCharacters* InCharacters)
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

TArray<FCharacterId> URtsPresence::CollectMembers(const FOrganizationId InOrganizationId, UCharacters* InCharacters)
{
	TArray<FCharacterId> Result;
	if (InCharacters == nullptr || !InOrganizationId.IsValid())
	{
		return Result;
	}

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
				Result.Add(Id);
				break;
			}
		}
	}
	return Result;
}
