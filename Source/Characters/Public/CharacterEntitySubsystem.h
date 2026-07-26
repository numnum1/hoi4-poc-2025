#pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "CharacterInfo.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "MassEntityQuery.h"
#include "Subsystems/WorldSubsystem.h"
#include "CharacterEntitySubsystem.generated.h"

class UCharacterHourlySimProcessor;
class UMassEntitySubsystem;
class UTime;
struct FMassEntityManager;
struct FPosition;

/**
 * Owns the character entities and bridges them to the stable FCharacterId.
 *
 * This is the template every other container (Provinces, Nations, Buildings, ...) should
 * follow when it moves to Mass: a world subsystem holding EntityByIndex, Create/Destroy
 * entry points, and FCharacterId-taking accessors so call sites outside Mass never need to
 * know about entity handles.
 *
 * Identity model: an id is a stable slot, never an array position. Once assigned it is never
 * reused, even after the character it names dies — DestroyCharacter empties the slot's handle
 * but the slot itself, and the character's display row, stay in place forever. NumSlots() is
 * the number of ids ever handed out; GetAliveIds() is the ones still walking around.
 */
UCLASS()
class CHARACTERS_API UCharacterEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one character entity and returns the id it claims. Ids are never reused. */
	FCharacterId CreateCharacter(const FCharacterSpawnData& InSpawnData);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyCharacter(FCharacterId InCharacterId);

	bool IsAlive(FCharacterId InCharacterId) const { return GetEntity(InCharacterId).IsValid(); }

	/** Starts driving the hourly simulation off InTime. Safe to call once per scenario load. */
	void BindToTime(UTime* InTime);

	FMassEntityHandle GetEntity(FCharacterId InCharacterId) const
	{
		return EntityByIndex.IsValidIndex(InCharacterId.Value) ? EntityByIndex[InCharacterId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FCharacterId>& GetAliveIds() const { return AliveIds; }

	// --- read ---
	FProvinceId GetCurrentProvince(FCharacterId InCharacterId) const;
	int32 GetTimezoneOffset(FCharacterId InCharacterId) const;
	FAbilities GetAbilities(FCharacterId InCharacterId) const;
	TArray<FPosition> GetPositions(FCharacterId InCharacterId) const;
	FSchedule GetSchedule(FCharacterId InCharacterId) const;
	/** The USchedules library entry this character's schedule came from. Display only. */
	FScheduleId GetScheduleId(FCharacterId InCharacterId) const;
	FCharacterVitals GetVitals(FCharacterId InCharacterId) const;
	FCharacterSkills GetSkills(FCharacterId InCharacterId) const;
	/** Every character currently living in InProvinceId. */
	TArray<FCharacterId> CollectCharactersInProvince(FProvinceId InProvinceId) const;

	// --- write ---
	/** Resolves the destination's timezone offset itself via UProvinceEntitySubsystem. */
	void MoveCharacter(FCharacterId InCharacterId, FProvinceId InNewProvinceId);
	void AddPosition(FCharacterId InCharacterId, FOrganizationId InAtOrganizationId);

	/**
	 * Fired once per character id the hourly processor just touched. Not a UPROPERTY —
	 * this is a world subsystem, not the Blueprint-facing facade; UCharacters subscribes once
	 * and re-broadcasts as its own BlueprintAssignable event so every UI hook lives in one place.
	 * Create/Destroy do not get an equivalent subsystem-level delegate: both only ever happen
	 * through UCharacters::Create/Destroy, so that facade broadcasts directly — a relay would
	 * fire before the facade's own display row exists.
	 */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterSimulated, FCharacterId /*InCharacterId*/);
	FOnCharacterSimulated OnCharacterSimulated;

	/** Logs one character's fragments. Backs the Rts.DumpCharacters console command. */
	void LogCharacter(FCharacterId InCharacterId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UFUNCTION()
	void HandleHourElapsed(int32 InHoursElapsed);

	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	UPROPERTY()
	TObjectPtr<UCharacterHourlySimProcessor> SimProcessor;

	UPROPERTY()
	TObjectPtr<UTime> Time;

	/** Id -> entity. The inverse lives on each entity as FCharacterIndexFragment. An invalid
	 * handle means that slot's character is dead; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FCharacterId> AliveIds;

	/** Standalone chunk-iteration query backing CollectCharactersInProvince. Not tied to a
	 * processor — see UCharacterEntitySubsystem::Initialize for how it is bound. Mutable: running
	 * it updates internal archetype caches even though CollectCharactersInProvince is const. */
	mutable FMassEntityQuery LocationQuery;
};
