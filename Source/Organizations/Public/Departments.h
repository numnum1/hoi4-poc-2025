#pragma once

#include "CoreMinimal.h"
#include "BuildingFunction.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Departments.generated.h"

class UDepartmentEntitySubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDepartmentEvent, FDepartmentId, InDepartmentId);

/**
 * The UI-facing face of the department data.
 *
 * Departments live in Mass entities owned by UDepartmentEntitySubsystem; this component adds
 * nothing of its own (there is no name or icon per department yet) beyond the change events the
 * view models bind to. Same layering as UOrganizations: Blueprint and view models talk to this,
 * never to the subsystem.
 *
 * The Collect* getters walk every department linearly, like UOrganizations' collectors — fine for
 * the UI layer this component exists for, never for a Mass processor.
 */
UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class ORGANIZATIONS_API UDepartments : public UActorComponent
{
	GENERATED_BODY()

public:
	UDepartments();

	UPROPERTY(BlueprintAssignable)
	FDepartmentEvent OnDepartmentCreated;
	/** Fired after a department's entity is gone. */
	UPROPERTY(BlueprintAssignable)
	FDepartmentEvent OnDepartmentDissolved;
	UPROPERTY(BlueprintAssignable)
	FDepartmentEvent OnDepartmentSeatChanged;
	UPROPERTY(BlueprintAssignable)
	FDepartmentEvent OnDepartmentParentChanged;

	/** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
	UFUNCTION(BlueprintPure)
	int32 Num() const;

	UFUNCTION(BlueprintPure)
	bool IsAlive(FDepartmentId InDepartmentId) const;

	UFUNCTION(BlueprintPure)
	TArray<FDepartmentId> GetAliveDepartments() const;

	UFUNCTION(BlueprintPure)
	FOrganizationId GetOrganization(FDepartmentId InDepartmentId) const;

	/** Invalid for a root department — the one its organization points at. */
	UFUNCTION(BlueprintPure)
	FDepartmentId GetParent(FDepartmentId InDepartmentId) const;

	/** The building the department sits in. It does not own that building. */
	UFUNCTION(BlueprintPure)
	FBuildingId GetSeat(FDepartmentId InDepartmentId) const;

	UFUNCTION(BlueprintPure)
	int32 GetSpace(FDepartmentId InDepartmentId) const;

	/** What the department does — the only thing that says what kind of thing it is. */
	UFUNCTION(BlueprintPure)
	EBuildingFunction GetFunction(FDepartmentId InDepartmentId) const;

	UFUNCTION(BlueprintPure)
	TArray<FDepartmentId> CollectDepartmentsOf(FOrganizationId InOrganizationId) const;

	UFUNCTION(BlueprintPure)
	TArray<FDepartmentId> CollectDepartmentsIn(FBuildingId InBuildingId) const;

	/** Direct children only, not the whole subtree. */
	UFUNCTION(BlueprintPure)
	TArray<FDepartmentId> CollectChildren(FDepartmentId InDepartmentId) const;

	/** How much space the departments already seated in InBuildingId claim for InFunction. Compare
	 * against UBuildingType::GetSpaceFor to find out whether another one still fits — a comparison
	 * for URtsActions, which is allowed to know both domains. */
	UFUNCTION(BlueprintPure)
	int32 GetUsedSpace(FBuildingId InBuildingId, EBuildingFunction InFunction) const;

	/** Creates a department and returns its id. Pass an invalid InParentId for the root one an
	 * organization is founded with. Whether the seat has room is the caller's check. */
	UFUNCTION(BlueprintCallable)
	FDepartmentId Add(FOrganizationId InOrganizationId, FDepartmentId InParentId, FBuildingId InSeatId,
	                  EBuildingFunction InFunction, int32 InSpace);

	UFUNCTION(BlueprintCallable)
	void SetParent(FDepartmentId InDepartmentId, FDepartmentId InNewParentId);

	UFUNCTION(BlueprintCallable)
	void SetSeat(FDepartmentId InDepartmentId, FBuildingId InBuildingId, int32 InSpace);

	/** Destroys the department. Its children keep pointing at it and have to be re-homed by
	 * whoever dissolves it — see UDepartmentEntitySubsystem::DestroyDepartment. */
	UFUNCTION(BlueprintCallable)
	void Dissolve(FDepartmentId InDepartmentId);

private:
	UDepartmentEntitySubsystem* GetEntities() const;
};
