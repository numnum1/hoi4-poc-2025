#pragma once

#include "CoreMinimal.h"
#include "PopulationInfo.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "MassEntityQuery.h"
#include "Subsystems/WorldSubsystem.h"
#include "PopulationEntitySubsystem.generated.h"

class UMassEntitySubsystem;
class UPopGrowthProcessor;
class UTime;
struct FMassEntityManager;

/**
 * Owns the pop entities and bridges them to the stable FPopId.
 *
 * One entity per province and ethnicity: a province where Dutch and German people live has two.
 * That split is the point — growth, migration and anything a pop later carries of its own become
 * plain chunk iteration instead of a loop inside a per-province array.
 */
UCLASS()
class POPULATIONS_API UPopulationEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one pop entity and returns the id it claims. Ids are never reused. InAnnualBirthRate
	 * is the resolved rate for this pop's ethnicity — this subsystem does not know the ethnicity
	 * library, so the caller (UPopulations::Create) looks it up first. */
	FPopId CreatePop(const FPopSpawnData& InSpawnData, float InAnnualBirthRate);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyPop(FPopId InPopId);

	bool IsAlive(FPopId InPopId) const { return GetEntity(InPopId).IsValid(); }

	/** Starts driving hourly growth off InTime. Safe to call once per scenario load. */
	void BindToTime(UTime* InTime);

	FMassEntityHandle GetEntity(FPopId InPopId) const
	{
		return EntityByIndex.IsValidIndex(InPopId.Value) ? EntityByIndex[InPopId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FPopId>& GetAliveIds() const { return AliveIds; }

	FPopInfo GetPopInfo(FPopId InPopId) const;
	int32 GetSize(FPopId InPopId) const;

	/** Every living pop, in one pass. The map's ethnicity mode wants the whole world at once, and
	 * asking province by province would rescan every pop per province. */
	TArray<FPopInfo> CollectPops() const;

	TArray<FPopInfo> CollectPopsInProvince(FProvinceId InProvinceId) const;

	/** Head count of a province across all its ethnicities. */
	int32 GetProvincePopulation(FProvinceId InProvinceId) const;

	void SetSize(FPopId InPopId, int32 InSize);

	/** Logs one pop's fragments. Backs the Rts.DumpPops console command. */
	void LogPop(FPopId InPopId) const;

	/**
	 * Fired once per HandleHourElapsed run that touched at least one pop — not per pop. Growth can
	 * move every pop in the world at once, and a payload-per-pop event would fire a whole world's
	 * worth of broadcasts every game hour. Not a UPROPERTY — this is a world subsystem, not the
	 * Blueprint-facing facade; UPopulations subscribes once and re-broadcasts as its own
	 * BlueprintAssignable event, same relay shape as UCharacterEntitySubsystem::OnCharacterSimulated.
	 */
	DECLARE_MULTICAST_DELEGATE(FOnPopulationsGrown);
	FOnPopulationsGrown OnPopulationsGrown;

	FMassEntityManager& GetEntityManager() const;

private:
	/** Runs InVisitor over every living pop's fragments. The one place the query is iterated, so the
	 * three collect-style getters cannot drift apart in what they consider a pop. */
	void ForEachPop(TFunctionRef<void(const FPopInfo&)> InVisitor) const;

	UFUNCTION()
	void HandleHourElapsed(int32 InHoursElapsed);

	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	UPROPERTY()
	TObjectPtr<UPopGrowthProcessor> SimProcessor;

	UPROPERTY()
	TObjectPtr<UTime> Time;

	/** Growth is applied once per crossed calendar month, not every hour — see HandleHourElapsed.
	 * This is the timestamp growth was last applied at; the gap to the current time is what the
	 * growth formula treats as elapsed. */
	FDateTime LastGrowthTime;

	/** Id -> entity. The inverse lives on each entity as FPopIndexFragment. An invalid handle means
	 * that slot's pop entity is gone; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	TArray<FPopId> AliveIds;

	/** Standalone chunk-iteration query backing the collect-style getters. Not tied to a processor —
	 * see Initialize for how it is bound. Mutable: running it updates internal archetype caches even
	 * though the getters are const. */
	mutable FMassEntityQuery PopQuery;
};
