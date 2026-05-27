#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Scenario.generated.h"

class UProvinceAsset;
class UCharacterAsset;

class FScenarioModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS(BlueprintType)
class SCENARIO_API UScenario : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UScenario();
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map", meta=(AllowPrivateAccess=true))
    TArray<UProvinceAsset*> Provinces;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Characters", meta=(AllowPrivateAccess=true))
    TArray<UCharacterAsset*> Characters;
public:
    FORCEINLINE const TArray<UProvinceAsset*>& GetProvinces() const { return Provinces; }
    FORCEINLINE const TArray<UCharacterAsset*>& GetCharacters() const { return Characters; }
};