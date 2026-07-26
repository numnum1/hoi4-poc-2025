#pragma once

#include "CoreMinimal.h"
#include "BuildingOwner.h"
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
 * to an actual UBuildingType*, and enforcing UBuildingType::GetMaxPerProvince(), to the facade
 * (UBuildings), which is the one holding the UObject pointers.
 */
UCLASS()
class BUILDINGS_API UBuildingEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one building entity and returns the id it claims. Ids are never reused. */
	FBuildingId CreateBuilding(FProvinceId InProvinceId, int32 InTypeIndex, const FBuildingOwner& InOwner);

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
	FBuildingOwner GetOwner(FBuildingId InBuildingId) const;
	/** Every building currently standing in InProvinceId. */
	TArray<FBuildingId> CollectBuildingsInProvince(FProvinceId InProvinceId) const;
	/** How many of InTypeIndex currently stand in InProvinceId. Backs the MaxPerProvince check. */
	int32 CountBuildingsOfType(FProvinceId InProvinceId, int32 InTypeIndex) const;

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

	/** Standalone chunk-iteration query backing CollectBuildingsInProvince/CountBuildingsOfType.
	 * Not tied to a processor — see Initialize for how it is bound. Mutable: running it updates
	 * internal archetype caches even though those calls are const. */
	mutable FMassEntityQuery LocationQuery;
};
