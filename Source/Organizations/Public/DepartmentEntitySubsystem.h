#pragma once

#include "CoreMinimal.h"
#include "BuildingFunction.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "MassEntityQuery.h"
#include "Subsystems/WorldSubsystem.h"
#include "DepartmentEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the department entities and bridges them to the stable FDepartmentId.
 *
 * Same shape as UOrganizationEntitySubsystem — departments are founded while playing, one at a
 * time, and ids are never reused. A department is the node an organization is actually made of:
 * the organization points at its root department, the department at its parent and at the building
 * it is seated in.
 *
 * Every relation is one field on the many-side and nothing else. Children, co-tenants of a
 * building and an organization's departments are all found by scanning, so no list can go stale.
 * Those scans are for the UI layer, never for a Mass processor.
 */
UCLASS()
class ORGANIZATIONS_API UDepartmentEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one department entity and returns the id it claims. An invalid InParentId makes it a
	 * root — the department an organization points at. */
	FDepartmentId CreateDepartment(FOrganizationId InOrganizationId, FDepartmentId InParentId,
	                               FBuildingId InSeatId, EBuildingFunction InFunction, int32 InSpace);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on.
	 * Children are not touched — they are left pointing at a dead parent rather than silently
	 * re-homed, because who inherits them is a gameplay decision, not a cleanup detail. */
	void DestroyDepartment(FDepartmentId InDepartmentId);

	bool IsAlive(FDepartmentId InDepartmentId) const { return GetEntity(InDepartmentId).IsValid(); }

	/** Destroys every department entity. For a scenario reload. */
	void Reset();

	FMassEntityHandle GetEntity(FDepartmentId InDepartmentId) const
	{
		return EntityByIndex.IsValidIndex(InDepartmentId.Value) ? EntityByIndex[InDepartmentId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FDepartmentId>& GetAliveIds() const { return AliveIds; }

	// --- read ---
	FOrganizationId GetOrganization(FDepartmentId InDepartmentId) const;
	/** Invalid for a root department. */
	FDepartmentId GetParent(FDepartmentId InDepartmentId) const;
	FBuildingId GetSeat(FDepartmentId InDepartmentId) const;
	int32 GetSpace(FDepartmentId InDepartmentId) const;
	EBuildingFunction GetFunction(FDepartmentId InDepartmentId) const;

	/** Every department belonging to InOrganizationId, root included. */
	TArray<FDepartmentId> CollectDepartmentsOf(FOrganizationId InOrganizationId) const;
	/** Every department seated in InBuildingId, whatever organization it belongs to. */
	TArray<FDepartmentId> CollectDepartmentsIn(FBuildingId InBuildingId) const;
	/** The direct children of InDepartmentId, not the whole subtree. */
	TArray<FDepartmentId> CollectChildren(FDepartmentId InDepartmentId) const;

	/** How much space the departments already seated in InBuildingId claim for InFunction. What is
	 * left is that minus UBuildingType::GetSpaceFor — a comparison only URtsActions can make, since
	 * it needs the building type this module deliberately cannot see. */
	int32 GetUsedSpace(FBuildingId InBuildingId, EBuildingFunction InFunction) const;

	// --- write ---
	/** Re-homes the department. Refuses a parent that is inside its own subtree, which would
	 * detach the whole branch from its organization and make every upward walk loop. */
	void SetParent(FDepartmentId InDepartmentId, FDepartmentId InNewParentId);
	/** Moves the department into another building. The space check belongs to the caller. */
	void SetSeat(FDepartmentId InDepartmentId, FBuildingId InBuildingId, int32 InSpace);

	/** Logs one department's fragments. Backs the Rts.DumpDepartments console command. */
	void LogDepartment(FDepartmentId InDepartmentId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FDepartmentIndexFragment. An invalid
	 * handle means that slot's department is gone; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FDepartmentId> AliveIds;

	/** Standalone chunk-iteration queries. Not tied to a processor — see Initialize for how they
	 * are bound. Mutable: running a query updates internal archetype caches even though the
	 * calling methods are const. */
	mutable FMassEntityQuery OrganizationQuery;
	mutable FMassEntityQuery ParentQuery;
	mutable FMassEntityQuery SeatQuery;
};
