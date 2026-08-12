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

	/** The map's third shading channel, one level above the province itself — an overlord, an
	 * alliance, whichever organization claims the area. Off means it follows PrimaryColor, so a
	 * province that belongs to nobody above it does not read as a bloc of its own. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseNationalColor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseNationalColor"))
	FColor NationalColor = FColor::Black;

	/** What actually goes into the secondary LUT — see bUseSecondaryColor. */
	FColor GetEffectiveSecondaryColor() const { return bUseSecondaryColor ? SecondaryColor : PrimaryColor; }

	/** What actually goes into the national LUT — see bUseNationalColor. */
	FColor GetEffectiveNationalColor() const { return bUseNationalColor ? NationalColor : PrimaryColor; }

	bool operator==(const FProvinceVisuals& Other) const
	{
		return PrimaryColor == Other.PrimaryColor
			&& GetEffectiveSecondaryColor() == Other.GetEffectiveSecondaryColor()
			&& GetEffectiveNationalColor() == Other.GetEffectiveNationalColor();
	}

	bool operator!=(const FProvinceVisuals& Other) const { return !(*this == Other); }
};
