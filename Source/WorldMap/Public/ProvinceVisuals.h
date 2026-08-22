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
	bool bUseSecondaryColor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseSecondaryColor"))
	FColor SecondaryColor = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseNationalColor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseNationalColor"))
	FColor NationalColor = FColor::Black;

	FColor GetEffectiveSecondaryColor() const { return bUseSecondaryColor ? SecondaryColor : PrimaryColor; }

	FColor GetEffectiveNationalColor() const { return bUseNationalColor ? NationalColor : PrimaryColor; }

	bool operator==(const FProvinceVisuals& Other) const
	{
		return PrimaryColor == Other.PrimaryColor
			&& GetEffectiveSecondaryColor() == Other.GetEffectiveSecondaryColor()
			&& GetEffectiveNationalColor() == Other.GetEffectiveNationalColor();
	}

	bool operator!=(const FProvinceVisuals& Other) const { return !(*this == Other); }
};
