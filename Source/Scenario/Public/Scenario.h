#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Scenario.generated.h"

class UProvinceAsset;
class UCharacterAsset;
class UOrganizationAsset;
class URoleAsset;
class UBankAsset;
class UBuildingType;
class UOrganizationFocusTreeAsset;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Organizations", meta=(AllowPrivateAccess=true))
    TArray<UOrganizationAsset*> Organizations;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Organizations", meta=(AllowPrivateAccess=true))
    TArray<URoleAsset*> Roles;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Banking", meta=(AllowPrivateAccess=true))
    TArray<UBankAsset*> Banks;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Buildings", meta=(AllowPrivateAccess=true))
    TArray<UBuildingType*> BuildingTypes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Focuses", meta=(AllowPrivateAccess=true))
    TArray<UOrganizationFocusTreeAsset*> FocusTrees;
public:
    FORCEINLINE const TArray<UProvinceAsset*>& GetProvinces() const { return Provinces; }
    FORCEINLINE const TArray<UCharacterAsset*>& GetCharacters() const { return Characters; }
    FORCEINLINE const TArray<UOrganizationAsset*>& GetOrganizations() const { return Organizations; }
    FORCEINLINE const TArray<URoleAsset*>& GetRoles() const { return Roles; }
    FORCEINLINE const TArray<UBankAsset*>& GetBanks() const { return Banks; }
    FORCEINLINE const TArray<UBuildingType*>& GetBuildingTypes() const { return BuildingTypes; }
    FORCEINLINE const TArray<UOrganizationFocusTreeAsset*>& GetFocusTrees() const { return FocusTrees; }
};