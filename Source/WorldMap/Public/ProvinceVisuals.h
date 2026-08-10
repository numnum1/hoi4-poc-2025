#pragma once

#include "CoreMinimal.h"
#include "ProvinceVisuals.generated.h"

USTRUCT(BlueprintType)
struct WORLDMAP_API FProvinceVisuals
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor PrimaryColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor SecondaryColor = FColor::Black;

	bool operator==(const FProvinceVisuals& Other) const
	{
		return PrimaryColor == Other.PrimaryColor && SecondaryColor == Other.SecondaryColor;
	}

	bool operator!=(const FProvinceVisuals& Other) const { return !(*this == Other); }
};
