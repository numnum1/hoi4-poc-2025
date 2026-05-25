 #pragma once

#include "CoreMinimal.h"
#include "CharacterInfo.generated.h"


USTRUCT(BlueprintType)
struct CHARACTERS_API FCharacterInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName = FText::FromString("Unnamed Character");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Portrait = FSlateNoResource();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BirthProvinceIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseRaceIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentProvinceIndex = 0;
	
};