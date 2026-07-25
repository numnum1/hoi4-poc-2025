#pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "CharacterInfo.h"
#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "CharacterEntitySubsystem.generated.h"

class UCharacterHourlySimProcessor;
class UMassEntitySubsystem;
class UTime;
struct FMassEntityManager;
struct FPosition;

/**
 * Owns the character entities and bridges them to the legacy int32 character index.
 *
 * This is the template every other container (Provinces, Nations, Buildings, ...) should
 * follow when it moves to Mass: a world subsystem holding EntityByIndex, a single Create
 * entry point fed from the scenario loader, and int32-taking accessors so call sites
 * outside Mass never need to know about entity handles.
 */
UCLASS()
class CHARACTERS_API UCharacterEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Creates one entity per entry, assigning index i to InSpawnData[i]. Replaces any existing set. */
	void CreateCharacters(const TArray<FCharacterSpawnData>& InSpawnData);

	/** Starts driving the hourly simulation off InTime. Safe to call once per scenario load. */
	void BindToTime(UTime* InTime);

	FMassEntityHandle GetEntity(int32 InCharacterIndex) const
	{
		return EntityByIndex.IsValidIndex(InCharacterIndex) ? EntityByIndex[InCharacterIndex] : FMassEntityHandle();
	}

	int32 Num() const { return EntityByIndex.Num(); }

	// --- read ---
	int32 GetCurrentProvince(int32 InCharacterIndex) const;
	int32 GetTimezoneOffset(int32 InCharacterIndex) const;
	FAbilities GetAbilities(int32 InCharacterIndex) const;
	TArray<FPosition> GetPositions(int32 InCharacterIndex) const;
	FSchedule GetSchedule(int32 InCharacterIndex) const;

	// --- write ---
	void MoveCharacter(int32 InCharacterIndex, int32 InNewProvinceIndex, int32 InNewTimezoneOffset);
	void AddPosition(int32 InCharacterIndex, int32 InAtOrganizationIndex);

	/** Logs one character's fragments. Backs the Rts.DumpCharacters console command. */
	void LogCharacter(int32 InCharacterIndex) const;

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

	/** Index -> entity. The inverse lives on each entity as FCharacterIndexFragment. */
	TArray<FMassEntityHandle> EntityByIndex;
};
