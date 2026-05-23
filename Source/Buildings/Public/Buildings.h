#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Modules/ModuleManager.h"
#include "Buildings.generated.h"

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
	
	UFUNCTION(BlueprintCallable)
	void InitWithProvinceAmount(int32 InProvinceAmount = 0);
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TArray<FBuilding> BuildingPool;
	// Number of buildings per province
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TArray<int32> BuildingCounts;
};