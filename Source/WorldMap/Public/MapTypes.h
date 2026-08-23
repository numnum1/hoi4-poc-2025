#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MapTypes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldMapProvinceClicked, FProvinceId, ProvinceId);

/**
 * A province's identity: red picks the column of the lookup textures, green the row, which is why
 * those textures are always 256 wide.
 *
 * This is NOT what the province-ID texture stores. There, green is stretched over the full byte
 * range so that it doubles as the V coordinate the material samples the lookup textures with — a
 * texel's green has to be scaled back by the highest row in use before it means anything here.
 * UWorldMapSubsystem::DecodePixel does that and is the only way a texel may become an FMapPixelId.
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldMapPixelClicked, FMapPixelId, PixelId);

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

/** One contiguous block of provinces a single label is laid out over: a nation split by water or by
 * another nation gets one zone per landmass, so its name is not drawn across the gap. Ported from the
 * plugin's FZone, with FProvinceId in place of the map editor's FName ids. */
USTRUCT(BlueprintType)
struct WORLDMAP_API FMapNameZone
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	TArray<FProvinceId> Provinces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	TArray<FProvinceId> Bordering;
};

/** What ARtsWorldMap needs to know about a nation to label it — nothing else, so WorldMap never has
 * to depend on the Nations module for just an id-to-name lookup. Pushed down from whatever knows
 * the actual nations (see URtsWorldMapViewModel::SetNationData). */
USTRUCT(BlueprintType)
struct WORLDMAP_API FWorldMapNationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	TArray<FProvinceId> OwnedProvinces;
};

/**
 * One finished country label, ready to be spawned: where it goes and what it says.
 *
 * The points are UVs rather than world positions so that whoever lays a label out never needs the
 * globe's radius — ARtsWorldMap projects them when it spawns the actor, because the radius is its
 * own business.
 */
USTRUCT(BlueprintType)
struct WORLDMAP_API FWorldMapLabel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	TArray<FVector2D> UVPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
	TArray<FNationId> OwnerTags;
};
