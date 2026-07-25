#pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "Characters.h"
#include "Schedule.h"
#include "Mass/EntityElementTypes.h"
#include "Mass/ExternalSubsystemTraits.h"   // TMassFragmentTraits
#include "CharacterFragments.generated.h"

// -----------------------------------------------------------------------------
// Identity
// -----------------------------------------------------------------------------

/**
 * Back-reference from an entity to the legacy int32 character index.
 * Everything outside Mass (FBuildingOwner, ARtsPlayer::ControlledCharacterIndex,
 * UOrganizations, the view models) still addresses characters by index, so every
 * character entity carries the index it was created for.
 */
USTRUCT()
struct CHARACTERS_API FCharacterIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Index = INDEX_NONE;
};

// -----------------------------------------------------------------------------
// Set once at creation
// -----------------------------------------------------------------------------

USTRUCT()
struct CHARACTERS_API FCharacterOriginFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 BirthProvinceIndex = 0;
	UPROPERTY()
	int32 BaseRaceIndex = 0;
};

// -----------------------------------------------------------------------------
// Mutated by gameplay
// -----------------------------------------------------------------------------

USTRUCT()
struct CHARACTERS_API FCharacterLocationFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 CurrentProvinceIndex = 0;
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

USTRUCT()
struct CHARACTERS_API FCharacterScheduleSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FSchedule Schedule;
};

USTRUCT()
struct CHARACTERS_API FCharacterRaceSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 RaceIndex = 0;
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
