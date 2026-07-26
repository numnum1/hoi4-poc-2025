#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "NationEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the nation entities and bridges them to the stable FNationId.
 *
 * Same shape as UCharacterEntitySubsystem/UProvinceEntitySubsystem/UOrganizationEntitySubsystem:
 * a world subsystem holding EntityByIndex, Create/Destroy entry points, and FNationId-taking
 * accessors so call sites outside Mass never need to know about entity handles. Nations carry
 * no gameplay fragments yet (see FNationTag) — this exists so future nation gameplay (diplomacy,
 * treasury, ...) has an entity to attach to instead of retrofitting one later.
 */
UCLASS()
class NATIONS_API UNationEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one nation entity and returns the id it claims. Ids are never reused. */
	FNationId CreateNation();

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyNation(FNationId InNationId);

	bool IsAlive(FNationId InNationId) const { return GetEntity(InNationId).IsValid(); }

	FMassEntityHandle GetEntity(FNationId InNationId) const
	{
		return EntityByIndex.IsValidIndex(InNationId.Value) ? EntityByIndex[InNationId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FNationId>& GetAliveIds() const { return AliveIds; }

	/** Logs one nation's fragments. Backs the Rts.DumpNations console command. */
	void LogNation(FNationId InNationId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FNationIndexFragment. An invalid
	 * handle means that slot's nation is gone; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FNationId> AliveIds;
};
