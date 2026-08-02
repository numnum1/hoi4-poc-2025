#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "BankEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the bank entities and bridges them to the stable FBankId.
 *
 * Same shape as UNationEntitySubsystem: banks carry no gameplay fragments yet (see FBankTag) —
 * this exists so future bank gameplay (interest rate, credit policy, ...) has an entity to attach
 * to instead of retrofitting one later.
 */
UCLASS()
class BANKING_API UBankEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one bank entity and returns the id it claims. Ids are never reused. */
	FBankId CreateBank();

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyBank(FBankId InBankId);

	bool IsAlive(FBankId InBankId) const { return GetEntity(InBankId).IsValid(); }

	FMassEntityHandle GetEntity(FBankId InBankId) const
	{
		return EntityByIndex.IsValidIndex(InBankId.Value) ? EntityByIndex[InBankId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FBankId>& GetAliveIds() const { return AliveIds; }

	/** Logs one bank's fragments. Backs the Rts.DumpBanks console command. */
	void LogBank(FBankId InBankId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FBankIndexFragment. An invalid handle
	 * means that slot's bank is gone; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FBankId> AliveIds;
};
