#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "Modules/ModuleManager.h"
#include "Provinces.generated.h"

class FProvincesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class PROVINCES_API UProvinces : public UActorComponent
{
    GENERATED_BODY()
public:
    UProvinces();
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    TArray<FProvinceInfo> Infos;
    // Optimization
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    TMap<FName, int32> MapIds;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    TArray<FName> OwningNations;
public:
    UFUNCTION(BlueprintCallable)
    FORCEINLINE int32 GetNumOfProvinces() const { return Infos.Num(); }
protected:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
private:
    UFUNCTION(CallInEditor)
    void InitValuesFromInfos();
};
