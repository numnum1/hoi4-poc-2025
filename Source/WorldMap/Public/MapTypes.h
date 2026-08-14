#pragma once

#include "CoreMinimal.h"
#include "MapTypes.generated.h"

/**
 * Globe is a sphere, FlatClamped a plane the camera cannot leave.
 *
 * The BloodAnvil plugin also had a looping "Flat" mode, which is deliberately not carried over: it
 * only worked alongside a second ghost actor mirroring the mesh at the seam, and nothing in this
 * game uses it.
 */
UENUM(BlueprintType)
enum class EMapType : uint8
{
	Globe,
	FlatClamped,
};

/**
 * A province's identity as the province-ID texture stores it: red and green carry the id, blue and
 * alpha are unused. Also the key the lookup textures are indexed by — red picks the column, green
 * the row, which is why those textures are always 256 wide.
 */
USTRUCT(BlueprintType)
struct WORLDMAP_API FMapPixelId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	uint8 R = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	uint8 G = 0;

	bool operator==(const FMapPixelId& InOther) const { return R == InOther.R && G == InOther.G; }
};

FORCEINLINE uint32 GetTypeHash(const FMapPixelId& InPixelId)
{
	return HashCombine(::GetTypeHash(InPixelId.R), ::GetTypeHash(InPixelId.G));
}

/** One province's row across the lookup textures, which are written together so a province's three
 * layers can never disagree about which pixel they belong to. */
USTRUCT(BlueprintType)
struct WORLDMAP_API FMapLutEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	FMapPixelId Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	FColor Primary = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	FColor Secondary = FColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	FColor National = FColor::Black;
};
