#pragma once

#include "CoreMinimal.h"
#include "CharacterSimTier.generated.h"

/**
 * How much simulation a character takes part in. Maps onto the Mass archetype:
 * Normal and Max each get their own tag, None gets none and is skipped by every processor.
 */
UENUM(BlueprintType)
enum class ECharacterSimTier : uint8
{
	CST_None UMETA(DisplayName="None", ToolTip="For only The Basics, No Simulation"),
	CST_Normal UMETA(DisplayName="Normal", ToolTip="For interactable characters"),
	CST_Max UMETA(DisplayName="Max", ToolTip="For active characters"),
};
