 #pragma once

#include "CoreMinimal.h"
#include "CharacterInfo.generated.h"


class URace;

UENUM(BlueprintType)
enum class ECharacterSimTier : uint8
{
	CST_None UMETA(DisplayName="None", ToolTip="For only The Basics, No Simulation"),
	CST_Normal UMETA(DisplayName="Normal", ToolTip="For interactable characters"),
	CST_Max UMETA(DisplayName="Max", ToolTip="For active characters"),
};

USTRUCT(BlueprintType)
struct CHARACTERS_API FCharacterInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Guid = FGuid();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterSimTier SimTier = ECharacterSimTier::CST_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName = FText::FromString("Unnamed Character");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Portrait = FSlateNoResource();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URace* BaseRace = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurrentProvince = NAME_None;
	
};