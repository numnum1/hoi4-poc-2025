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

UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class ORGANIZATIONS_API UOrganizations : public UActorComponent
{
    GENERATED_BODY()
public:
    UOrganizations();
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
    TArray<FOrganizationInfo> Infos;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
    TArray<FSettledNations> SettledNations;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
    TArray<FOrganizationAlignment> Alignments; 
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE FOrganizationInfo GetOrganizationInfo(const int32 InIndex) const { return Infos[InIndex]; }
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    int32 Add(const FOrganizationInfo& NewOrg);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void PublishAlignment(int32 InOrganizationIndex, const TArray<float>& InAlignment);
};