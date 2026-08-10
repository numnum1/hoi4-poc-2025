#pragma once

#include "CoreMinimal.h"
#include "BuildingFunction.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "DepartmentViewModel.generated.h"

class UBuildings;
class UDepartments;

/**
 * A read-only C++ MVVM view model for a single department, addressed by the same FDepartmentId
 * every other department API uses. Same pattern as UOrganizationViewModel: resolves its facades
 * off the game state, binds directly to UDepartments' events (filtered to this id), and
 * refreshes flat FieldNotify properties on every change.
 *
 * Reads UBuildings as well as UDepartments, for the name of the building the department sits in —
 * the same cross-domain hop UCharacterViewModel makes, and the reason these view models live in
 * RtsUI rather than in a domain module (only RtsUI is allowed to know every facade at once).
 *
 * Function is mirrored as an int32 next to the enum because UMG's conversion nodes work on int32.
 *
 * A Manual row, not CreateInstance: a department only ever appears as one entry in its
 * organization's department list, never as a standalone Blueprint view model — see
 * UOrganizationViewModel::Departments, which owns and caches one instance per id and calls
 * SetSource directly, the same shape USelectablePositionViewModel::SetSource has.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UDepartmentViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Pushed by the owning UOrganizationViewModel: which department this row stands for. */
	void SetSource(FDepartmentId InDepartmentId) { Initialize(InDepartmentId); }

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False once the department's entity has been dissolved. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Department Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel", meta=(AllowPrivateAccess))
	FDepartmentId DepartmentId;

	/** DepartmentId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel", meta=(AllowPrivateAccess))
	int32 DepartmentIdValue = INDEX_NONE;

	//~ Place in the organization ----------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Organization", meta=(AllowPrivateAccess))
	FOrganizationId OrganizationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Organization", meta=(AllowPrivateAccess))
	int32 OrganizationIdValue = INDEX_NONE;

	/** Invalid for the root department. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Organization", meta=(AllowPrivateAccess))
	FDepartmentId ParentDepartmentId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Organization", meta=(AllowPrivateAccess))
	int32 ParentDepartmentIdValue = INDEX_NONE;

	/** True when this is the department its organization is built around. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsRoot, Category="Department Viewmodel|Organization", meta=(AllowPrivateAccess))
	bool bIsRoot = false;

	//~ Seat -------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Seat", meta=(AllowPrivateAccess))
	FBuildingId SeatBuildingId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Seat", meta=(AllowPrivateAccess))
	int32 SeatBuildingIdValue = INDEX_NONE;

	/** The name of the building type the department sits in. Empty while the seat has no type. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Seat", meta=(AllowPrivateAccess))
	FText SeatBuildingName;

	/** How much of the seat's space for this function the department takes up. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Seat", meta=(AllowPrivateAccess))
	int32 Space = 0;

	//~ Function ---------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Function", meta=(AllowPrivateAccess))
	EBuildingFunction Function = EBuildingFunction::BF_Bureaucracy;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Function", meta=(AllowPrivateAccess))
	int32 FunctionValue = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Department Viewmodel|Function", meta=(AllowPrivateAccess))
	FText FunctionName;

public:
	bool IsValid() const { return bIsValid; }
	FDepartmentId GetDepartmentId() const { return DepartmentId; }
	int32 GetDepartmentIdValue() const { return DepartmentIdValue; }

	FOrganizationId GetOrganizationId() const { return OrganizationId; }
	int32 GetOrganizationIdValue() const { return OrganizationIdValue; }
	FDepartmentId GetParentDepartmentId() const { return ParentDepartmentId; }
	int32 GetParentDepartmentIdValue() const { return ParentDepartmentIdValue; }
	bool IsRoot() const { return bIsRoot; }

	FBuildingId GetSeatBuildingId() const { return SeatBuildingId; }
	int32 GetSeatBuildingIdValue() const { return SeatBuildingIdValue; }
	FText GetSeatBuildingName() const { return SeatBuildingName; }
	int32 GetSpace() const { return Space; }

	EBuildingFunction GetFunction() const { return Function; }
	int32 GetFunctionValue() const { return FunctionValue; }
	FText GetFunctionName() const { return FunctionName; }

protected:
	UFUNCTION(BlueprintCallable, Category="Department Viewmodel")
	void Initialize(FDepartmentId InDepartmentId);

	UFUNCTION(BlueprintNativeEvent, Category="Department Viewmodel")
	void Refresh();

	//~ Begin UObject interface
	// Protected rather than private so a subclass in RtsUI can call Super::BeginDestroy() and let
	// this class unbind — the same reason UCharacterViewModel keeps it protected.
	virtual void BeginDestroy() override;
	//~ End UObject interface

private:
	void Unbind();

	UFUNCTION()
	void HandleDepartmentEvent(FDepartmentId InDepartmentId);

	UPROPERTY()
	TObjectPtr<UDepartments> Departments;

	UPROPERTY()
	TObjectPtr<UBuildings> Buildings;
};
