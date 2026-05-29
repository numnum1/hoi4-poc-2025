#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "Modules/ModuleManager.h"
#include "UObject/CoreNet.h"
#include "Organizations.generated.h"

class FOrganizationsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct ORGANIZATIONS_API FSettledNations
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> Value = {};
};

USTRUCT(BlueprintType)
struct ORGANIZATIONS_API FOrganizationAlignment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPublished = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> Branches = {};
    
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrganizationsEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOrganizationEvent, int32, InOrganizationIndex);

UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class ORGANIZATIONS_API UOrganizations : public UActorComponent
{
    GENERATED_BODY()
public:
    UOrganizations();

    UPROPERTY(BlueprintAssignable)
    FOrganizationsEvent OnOrganizationAdded;
    // Org events
    UPROPERTY(BlueprintAssignable)
    FOrganizationEvent OnNationSettled;
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    TArray<FOrganizationInfo> Infos;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    TArray<FSettledNations> SettledNations;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
    TArray<FOrganizationAlignment> Alignments; 
public:
    UFUNCTION(BlueprintCallable)
    FORCEINLINE FOrganizationInfo GetOrganizationInfo(const int32 InIndex) const { return Infos[InIndex]; }
    
    UFUNCTION(BlueprintCallable)
    int32 Add(const FOrganizationInfo& NewOrg);
    
    UFUNCTION(BlueprintCallable)
    void PublishAlignment(int32 InOrganizationIndex, const TArray<float>& InAlignment);
    
    UFUNCTION(BlueprintCallable)
    void SettleNation(int32 InOrganizationIndex, int32 InNationIndex);
    
};