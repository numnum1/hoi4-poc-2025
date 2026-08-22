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
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void Refresh();
};
