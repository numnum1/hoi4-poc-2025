#include "Departments.h"

#include "DepartmentEntitySubsystem.h"
#include "Engine/World.h"

UDepartments::UDepartments()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

UDepartmentEntitySubsystem* UDepartments::GetEntities() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UDepartmentEntitySubsystem>() : nullptr;
}

int32 UDepartments::Num() const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->NumSlots() : 0;
}

bool UDepartments::IsAlive(const FDepartmentId InDepartmentId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities && Entities->IsAlive(InDepartmentId);
}

TArray<FDepartmentId> UDepartments::GetAliveDepartments() const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetAliveIds() : TArray<FDepartmentId>();
}

FOrganizationId UDepartments::GetOrganization(const FDepartmentId InDepartmentId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetOrganization(InDepartmentId) : FOrganizationId();
}

FDepartmentId UDepartments::GetParent(const FDepartmentId InDepartmentId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetParent(InDepartmentId) : FDepartmentId();
}

FBuildingId UDepartments::GetSeat(const FDepartmentId InDepartmentId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetSeat(InDepartmentId) : FBuildingId();
}

int32 UDepartments::GetSpace(const FDepartmentId InDepartmentId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetSpace(InDepartmentId) : 0;
}

EBuildingFunction UDepartments::GetFunction(const FDepartmentId InDepartmentId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetFunction(InDepartmentId) : EBuildingFunction::BF_Bureaucracy;
}

TArray<FDepartmentId> UDepartments::CollectDepartmentsOf(const FOrganizationId InOrganizationId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectDepartmentsOf(InOrganizationId) : TArray<FDepartmentId>();
}

TArray<FDepartmentId> UDepartments::CollectDepartmentsIn(const FBuildingId InBuildingId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectDepartmentsIn(InBuildingId) : TArray<FDepartmentId>();
}

TArray<FDepartmentId> UDepartments::CollectChildren(const FDepartmentId InDepartmentId) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->CollectChildren(InDepartmentId) : TArray<FDepartmentId>();
}

int32 UDepartments::GetUsedSpace(const FBuildingId InBuildingId, const EBuildingFunction InFunction) const
{
	const UDepartmentEntitySubsystem* Entities = GetEntities();
	return Entities ? Entities->GetUsedSpace(InBuildingId, InFunction) : 0;
}

FDepartmentId UDepartments::Add(const FOrganizationId InOrganizationId, const FDepartmentId InParentId,
                                const FBuildingId InSeatId, const EBuildingFunction InFunction, const int32 InSpace)
{
	UDepartmentEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return FDepartmentId();
	}

	const FDepartmentId NewId = Entities->CreateDepartment(InOrganizationId, InParentId, InSeatId, InFunction, InSpace);
	OnDepartmentCreated.Broadcast(NewId);
	return NewId;
}

void UDepartments::SetParent(const FDepartmentId InDepartmentId, const FDepartmentId InNewParentId)
{
	UDepartmentEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return;
	}

	Entities->SetParent(InDepartmentId, InNewParentId);
	OnDepartmentParentChanged.Broadcast(InDepartmentId);
}

void UDepartments::SetSeat(const FDepartmentId InDepartmentId, const FBuildingId InBuildingId, const int32 InSpace)
{
	UDepartmentEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return;
	}

	Entities->SetSeat(InDepartmentId, InBuildingId, InSpace);
	OnDepartmentSeatChanged.Broadcast(InDepartmentId);
}

void UDepartments::Dissolve(const FDepartmentId InDepartmentId)
{
	UDepartmentEntitySubsystem* Entities = GetEntities();
	if (!Entities)
	{
		return;
	}

	Entities->DestroyDepartment(InDepartmentId);
	OnDepartmentDissolved.Broadcast(InDepartmentId);
}
