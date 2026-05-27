#pragma once

#include "CoreMinimal.h"
#include "BuildingOwner.h"
#include "Modules/ModuleManager.h"
#include "Buildings.generated.h"

class UBuildingType;

class FBuildingsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

constexpr int32 MAX_BUILDINGS_PER_PROVINCE = 50; // Dont set this smaller than 0

/**
 * 
 */
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class BUILDINGS_API UBuildings : public UActorComponent
{
	GENERATED_BODY()
public:
	UBuildings();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
	TArray<UBuildingType*> BuildingPool;
	// Number of buildings per province
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
	TArray<int32> BuildingCounts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
	TArray<FBuildingOwner> BuildingOwners;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FORCEINLINE const TArray<UBuildingType*>& GetBuildingPool() const { return BuildingPool; }
	FORCEINLINE const TArray<int32>& GetBuildingCounts() const { return BuildingCounts; }
	
	UFUNCTION(BlueprintCallable)
	void Add(UBuildingType* InBuildingType, int32 InProvinceIndex, const FBuildingOwner& InOwner);
	
	UFUNCTION(BlueprintCallable)
	void InitWithProvinceAmount(int32 InProvinceAmount = 0);
};