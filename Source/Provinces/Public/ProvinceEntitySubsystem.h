#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "MassEntityQuery.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProvinceEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the province entities and bridges them to the stable FProvinceId.
 *
 * Server-only in effect: Mass entities do not replicate, so clients read province ownership
 * from the replicated mirror on UProvinces instead. See the comment there.
 */
UCLASS()
class PROVINCES_API UProvinceEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one province entity and returns the id it claims. Ids are never reused. */
	FProvinceId CreateProvince(const FProvinceSpawnData& InSpawnData);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyProvince(FProvinceId InProvinceId);

	bool IsAlive(FProvinceId InProvinceId) const { return GetEntity(InProvinceId).IsValid(); }

	FMassEntityHandle GetEntity(FProvinceId InProvinceId) const
	{
		return EntityByIndex.IsValidIndex(InProvinceId.Value) ? EntityByIndex[InProvinceId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FProvinceId>& GetAliveIds() const { return AliveIds; }

	// --- read ---
	FNationId GetOwningNation(FProvinceId InProvinceId) const;
	int32 GetTimezoneOffset(FProvinceId InProvinceId) const;
	/** Every province currently owned by InNationId. */
	TArray<FProvinceId> CollectProvincesOfNation(FNationId InNationId) const;

	// --- write ---
	void SetOwningNation(FProvinceId InProvinceId, FNationId InNationId);

	/** Logs one province's fragments. Backs the Rts.DumpProvinces console command. */
	void LogProvince(FProvinceId InProvinceId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FProvinceIndexFragment. An invalid
	 * handle means that slot's province entity is gone; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FProvinceId> AliveIds;

	/** Standalone chunk-iteration query backing CollectProvincesOfNation. Not tied to a
	 * processor — see UProvinceEntitySubsystem::Initialize for how it is bound. Mutable: running
	 * it updates internal archetype caches even though CollectProvincesOfNation is const. */
	mutable FMassEntityQuery OwnershipQuery;
};
