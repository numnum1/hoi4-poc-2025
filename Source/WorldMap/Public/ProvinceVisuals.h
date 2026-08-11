#pragma once

#include "CoreMinimal.h"
#include "ProvinceVisuals.generated.h"

USTRUCT(BlueprintType)
struct WORLDMAP_API FProvinceVisuals
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor PrimaryColor = FColor::Black;

	/** Off means the province renders flat: SecondaryColor is ignored rather than written as
	 * black, which the map's shading would show as a stripe nobody asked for. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseSecondaryColor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseSecondaryColor"))
	FColor SecondaryColor = FColor::Black;

	/** What actually goes into the secondary LUT — see bUseSecondaryColor. */
	FColor GetEffectiveSecondaryColor() const { return bUseSecondaryColor ? SecondaryColor : PrimaryColor; }

	bool operator==(const FProvinceVisuals& Other) const
	{
		return PrimaryColor == Other.PrimaryColor
			&& GetEffectiveSecondaryColor() == Other.GetEffectiveSecondaryColor();
	}

	bool operator!=(const FProvinceVisuals& Other) const { return !(*this == Other); }
};
