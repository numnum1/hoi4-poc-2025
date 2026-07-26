#pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "Characters.h"
#include "RtsIds.h"
#include "Schedule.h"
#include "Mass/EntityElementTypes.h"
#include "Mass/ExternalSubsystemTraits.h"   // TMassFragmentTraits
#include "CharacterFragments.generated.h"

// -----------------------------------------------------------------------------
// Identity
// -----------------------------------------------------------------------------

/**
 * Back-reference from an entity to its stable FCharacterId.
 * Everything outside Mass (FBuildingOwner, ARtsPlayer::ControlledCharacterId,
 * UOrganizations, the view models) still addresses characters by this id, so every
 * character entity carries the id it was created for.
 */
USTRUCT()
struct CHARACTERS_API FCharacterIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FCharacterId Id;
};

// -----------------------------------------------------------------------------
// Set once at creation
// -----------------------------------------------------------------------------

USTRUCT()
struct CHARACTERS_API FCharacterOriginFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FProvinceId BirthProvinceId;
	UPROPERTY()
	FRaceId BaseRaceId;
};

// -----------------------------------------------------------------------------
// Mutated by gameplay
// -----------------------------------------------------------------------------

USTRUCT()
struct CHARACTERS_API FCharacterLocationFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FProvinceId CurrentProvinceId;
	/** Cached from the current province so the sim processor needs no province lookup. */
	UPROPERTY()
	int32 TimezoneOffset = 0;
};

USTRUCT()
struct CHARACTERS_API FCharacterAbilitiesFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FAbilities Abilities;
};

USTRUCT()
struct CHARACTERS_API FCharacterVitalsFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Energy = 100;
	UPROPERTY()
	int32 Happiness = 50;
};

USTRUCT()
struct CHARACTERS_API FCharacterSkillsFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Science = 0;
	UPROPERTY()
	int32 Strength = 0;
	UPROPERTY()
	int32 Medicine = 0;
};

USTRUCT()
struct CHARACTERS_API FCharacterPositionsFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPosition> Values;
};

// A character holds an open-ended number of organization positions, so this one fragment
// cannot be a flat POD like the rest. Mass wants that said out loud.
template<>
struct TMassFragmentTraits<FCharacterPositionsFragment> final
{
	enum
	{
		AuthorAcceptsItsNotTriviallyCopyable = true
	};
};

// -----------------------------------------------------------------------------
// Shared: many characters reference the same schedule / race data
// -----------------------------------------------------------------------------

/**
 * The schedule a character runs, by value. Const shared: Mass hashes the whole fragment, so
 * every character following the same library entry lands on the same instance automatically.
 * ScheduleId rides along for display only — same asset means same id means same schedule, so
 * it never splits the dedup any finer than the schedule already does.
 */
USTRUCT()
struct CHARACTERS_API FCharacterScheduleSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FSchedule Schedule;

	UPROPERTY()
	FScheduleId ScheduleId;
};

USTRUCT()
struct CHARACTERS_API FCharacterRaceSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FRaceId RaceId;
	UPROPERTY()
	FAbilities RaceModifiers;
};

// -----------------------------------------------------------------------------
// Tags — ECharacterSimTier becomes the archetype split
// -----------------------------------------------------------------------------

USTRUCT()
struct CHARACTERS_API FCharacterTag : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT()
struct CHARACTERS_API FCharacterSimNormalTag : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT()
struct CHARACTERS_API FCharacterSimMaxTag : public FMassTag
{
	GENERATED_BODY()
};
// CST_None deliberately gets no tag: the absence of both means "not simulated".
