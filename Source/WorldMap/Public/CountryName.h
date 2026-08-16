#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CountryName.generated.h"

class ARtsWorldMap;

/**
 * A country's name label on the globe. ARtsWorldMap decides where a name belongs and spawns one of
 * these per label; this actor only holds what it was given and displays it.
 */
UCLASS()
class WORLDMAP_API ACountryName : public AActor
{
	GENERATED_BODY()

public:
	ACountryName();

	/** Points is the path the name is laid out along (e.g. the country's outline or a fitted
	 * centerline) in world space; OwnerTags is whatever ARtsWorldMap's caller used to mark the
	 * country the points belong to, carried along for later filtering (e.g. by nation). */
	UFUNCTION(BlueprintCallable, Category="Country Name")
	void Initialize(const TArray<FVector>& InPoints, const FText& InName, ARtsWorldMap* InMap, const TArray<FNationId>& InOwnerTags);

	UPROPERTY(BlueprintReadOnly, Category="Country Name")
	TArray<FVector> Points;

	UPROPERTY(BlueprintReadOnly, Category="Country Name")
	FText Name;

	UPROPERTY(BlueprintReadOnly, Category="Country Name")
	TObjectPtr<ARtsWorldMap> Map;

	UPROPERTY(BlueprintReadOnly, Category="Country Name")
	TArray<FNationId> OwnerTags;
};
