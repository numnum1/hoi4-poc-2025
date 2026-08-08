#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "MassEntityQuery.h"
#include "Subsystems/WorldSubsystem.h"
#include "BuildingEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the building entities and bridges them to the stable FBuildingId.
 *
 * Same shape as the other domain subsystems. Deliberately knows nothing about UBuildingType —
 * it stores an opaque TypeIndex per entity (see FBuildingTypeFragment) and leaves resolving that
 * to an actual UBuildingType* to the facade (UBuildings), which is the one holding the UObject
 * pointers.
 *
 * A building always belongs to a character. Organizations do not own buildings: an organization
 * has a root department, and a department is seated in a building somebody else owns — see
 * FDepartmentSeatFragment. That is why there is one owner fragment here and no owner-kind enum.
 */
UCLASS()
class BUILDINGS_API UBuildingEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one building entity owned by a character and returns the id it claims. Ids are
	 * never reused. */
	FBuildingId CreateBuilding(FProvinceId InProvinceId, int32 InTypeIndex, FCharacterId InOwnerId);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyBuilding(FBuildingId InBuildingId);

	bool IsAlive(FBuildingId InBuildingId) const { return GetEntity(InBuildingId).IsValid(); }

	FMassEntityHandle GetEntity(FBuildingId InBuildingId) const
	{
		return EntityByIndex.IsValidIndex(InBuildingId.Value) ? EntityByIndex[InBuildingId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FBuildingId>& GetAliveIds() const { return AliveIds; }

	// --- read ---
	FProvinceId GetProvince(FBuildingId InBuildingId) const;
	int32 GetTypeIndex(FBuildingId InBuildingId) const;
	/** The character the building belongs to, or an invalid id if InBuildingId is not alive. */
	FCharacterId GetOwnerCharacter(FBuildingId InBuildingId) const;

	/** Every building currently standing in InProvinceId. */
	TArray<FBuildingId> CollectBuildingsInProvince(FProvinceId InProvinceId) const;
	/** Every building currently owned by InOwnerId. */
	TArray<FBuildingId> CollectBuildingsOwnedByCharacter(FCharacterId InOwnerId) const;

	/** Hands the building to a different character. A plain value write — every building has the
	 * owner fragment from creation on, so there is no archetype move. */
	void SetCharacterOwner(FBuildingId InBuildingId, FCharacterId InNewOwnerId);

	/** Logs one building's fragments. Backs the Rts.DumpBuildings console command. */
	void LogBuilding(FBuildingId InBuildingId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FBuildingIndexFragment. An invalid
	 * handle means that slot's building is gone; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FBuildingId> AliveIds;

	/** Standalone chunk-iteration queries. Not tied to a processor — see Initialize for how they
	 * are bound. Mutable: running a query updates internal archetype caches even though the
	 * calling methods are const. */
	mutable FMassEntityQuery LocationQuery;
	mutable FMassEntityQuery CharacterOwnerQuery;
};
