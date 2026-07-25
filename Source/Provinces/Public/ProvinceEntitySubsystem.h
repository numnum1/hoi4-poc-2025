#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProvinceEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the province entities and bridges them to the legacy int32 province index.
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

	/** Creates one entity per entry, assigning index i to InSpawnData[i]. Replaces any existing set. */
	void CreateProvinces(const TArray<FProvinceSpawnData>& InSpawnData);

	FMassEntityHandle GetEntity(int32 InProvinceIndex) const
	{
		return EntityByIndex.IsValidIndex(InProvinceIndex) ? EntityByIndex[InProvinceIndex] : FMassEntityHandle();
	}

	int32 Num() const { return EntityByIndex.Num(); }

	// --- read ---
	int32 GetOwningNation(int32 InProvinceIndex) const;
	/** Every province index currently owned by InNationIndex. */
	TArray<int32> CollectProvincesOfNation(int32 InNationIndex) const;

	// --- write ---
	void SetOwningNation(int32 InProvinceIndex, int32 InNationIndex);

	/** Logs one province's fragments. Backs the Rts.DumpProvinces console command. */
	void LogProvince(int32 InProvinceIndex) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Index -> entity. The inverse lives on each entity as FProvinceIndexFragment. */
	TArray<FMassEntityHandle> EntityByIndex;
};
