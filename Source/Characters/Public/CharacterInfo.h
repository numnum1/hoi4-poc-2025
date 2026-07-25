 #pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "CharacterSimTier.h"
#include "Schedule.h"
#include "CharacterInfo.generated.h"


/**
 * The UI-only half of a character: heavyweight types that deliberately stay out of Mass.
 * FSlateBrush holds a TObjectPtr the entity manager would not add to the GC graph, and
 * FText is a refcounted type with no business in a per-entity loop. Lives in UCharacters,
 * indexed by the same int32 that FCharacterIndexFragment carries.
 */
USTRUCT(BlueprintType)
struct CHARACTERS_API FCharacterDisplayInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName = FText::FromString("Unnamed Character");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Portrait = FSlateNoResource();
};

/**
 * Everything needed to create one character entity plus its display row.
 * Produced by ScenarioLoader::ExtractCharacters, consumed by
 * UCharacterEntitySubsystem::CreateCharacters and UCharacters::SetupDisplay.
 */
USTRUCT(BlueprintType)
struct CHARACTERS_API FCharacterSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterDisplayInfo Display;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BirthProvinceIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseRaceIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentProvinceIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimezoneOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSchedule Schedule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAbilities RaceModifiers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterSimTier SimTier = ECharacterSimTier::CST_Normal;
};
