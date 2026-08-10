#include "Organizations/DepartmentViewModel.h"

#include "Buildings.h"
#include "BuildingType.h"
#include "Departments.h"
#include "RtsGameStateComponent.h"

void UDepartmentViewModel::Initialize(const FDepartmentId InDepartmentId)
{
	UDepartments* NewDepartments = FindRtsGameStateComponent<UDepartments>(this);
	UBuildings* NewBuildings = FindRtsGameStateComponent<UBuildings>(this);

	if (Departments == NewDepartments && DepartmentId == InDepartmentId)
	{
		return;
	}

	Unbind();

	Departments = NewDepartments;
	Buildings = NewBuildings;

	DepartmentId = InDepartmentId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(DepartmentId);
	UE_MVVM_SET_PROPERTY_VALUE(DepartmentIdValue, DepartmentId.Value);

	if (Departments)
	{
		Departments->OnDepartmentDissolved.AddDynamic(this, &UDepartmentViewModel::HandleDepartmentEvent);
		Departments->OnDepartmentSeatChanged.AddDynamic(this, &UDepartmentViewModel::HandleDepartmentEvent);
		Departments->OnDepartmentParentChanged.AddDynamic(this, &UDepartmentViewModel::HandleDepartmentEvent);
	}

	Refresh();
}

void UDepartmentViewModel::HandleDepartmentEvent(const FDepartmentId InDepartmentId)
{
	if (InDepartmentId == DepartmentId)
	{
		Refresh();
	}
}

void UDepartmentViewModel::Refresh_Implementation()
{
	if (Departments == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Departments->IsAlive(DepartmentId));

	const FOrganizationId NewOrganizationId = Departments->GetOrganization(DepartmentId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationId, NewOrganizationId);
	UE_MVVM_SET_PROPERTY_VALUE(OrganizationIdValue, NewOrganizationId.Value);

	const FDepartmentId NewParentId = Departments->GetParent(DepartmentId);
	UE_MVVM_SET_PROPERTY_VALUE(ParentDepartmentId, NewParentId);
	UE_MVVM_SET_PROPERTY_VALUE(ParentDepartmentIdValue, NewParentId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(bIsRoot, !NewParentId.IsValid());

	const FBuildingId NewSeatId = Departments->GetSeat(DepartmentId);
	UE_MVVM_SET_PROPERTY_VALUE(SeatBuildingId, NewSeatId);
	UE_MVVM_SET_PROPERTY_VALUE(SeatBuildingIdValue, NewSeatId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(Space, Departments->GetSpace(DepartmentId));

	const UBuildingType* SeatType = Buildings ? Buildings->GetBuildingType(NewSeatId) : nullptr;
	UE_MVVM_SET_PROPERTY_VALUE(SeatBuildingName, SeatType ? SeatType->GetBuildingName() : FText::GetEmpty());

	const EBuildingFunction NewFunction = Departments->GetFunction(DepartmentId);
	UE_MVVM_SET_PROPERTY_VALUE(Function, NewFunction);
	UE_MVVM_SET_PROPERTY_VALUE(FunctionValue, static_cast<int32>(NewFunction));
	UE_MVVM_SET_PROPERTY_VALUE(FunctionName, UEnum::GetDisplayValueAsText(NewFunction));
}

void UDepartmentViewModel::Unbind()
{
	if (Departments)
	{
		Departments->OnDepartmentDissolved.RemoveAll(this);
		Departments->OnDepartmentSeatChanged.RemoveAll(this);
		Departments->OnDepartmentParentChanged.RemoveAll(this);
	}
}

void UDepartmentViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
