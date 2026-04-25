#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Organisation.generated.h"

class UOrganizationPosition;

class FOrganisationModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct ORGANISATION_API FOrganizationInfo
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name = FText::FromString("Unnamed Organisation");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlateBrush Logo = FSlateNoResource();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrganisationEvent);

UCLASS()
class ORGANISATION_API AOrganization : public AActor
{
    GENERATED_BODY()
    
public:
    AOrganization();

    UPROPERTY(BlueprintAssignable)
    FOrganisationEvent OnInfoChanged;
    
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintGetter=GetInfo, BlueprintSetter=SetInfo, Replicated, ReplicatedUsing=OnRep_Info)
    FOrganizationInfo Info;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
    TArray<AActor*> PositionHolders;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Actions
    UFUNCTION(BlueprintCallable)
    void BuyProperty();
    
    UFUNCTION(BlueprintCallable)
    void RegisterNewWorker(AActor* InNewPositionHolder);
    
    UFUNCTION(BlueprintGetter)
    const FOrganizationInfo& GetInfo() const
    {
        return Info;
    }
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetInfo(const FOrganizationInfo& NewInfo)
    {
        Info = NewInfo;
        OnInfoChanged.Broadcast();
    }
private:
    UFUNCTION()
    FORCEINLINE void OnRep_Info()
    {
        OnInfoChanged.Broadcast();
    }
};
