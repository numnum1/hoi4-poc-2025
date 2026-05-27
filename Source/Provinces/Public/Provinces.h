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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Replicated, ReplicatedUsing=OnRep_Infos)
    TArray<FProvinceInfo> Infos;
    // Optimization
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TMap<FName, int32> MapIds;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<int32> OwningNationIndexes;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void Setup(const TArray<FProvinceInfo>& NewInfos);
    
private:
    UFUNCTION()
    void OnRep_Infos();
};
