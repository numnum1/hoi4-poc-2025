#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Organisation.generated.h"

class FOrganisationModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct ORGANISATION_API FOrganisationInfo
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name = FText::FromString("Unnamed Organisation");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlateBrush Logo = FSlateNoResource();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrganisationEvent);

UCLASS(Abstract)
class ORGANISATION_API AOrganisation : public AActor
{
    GENERATED_BODY()
    
public:
    AOrganisation();

    UPROPERTY(BlueprintAssignable)
    FOrganisationEvent OnInfoChanged;
    
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintGetter=GetInfo, BlueprintSetter=SetInfo, Replicated, ReplicatedUsing=OnRep_Info)
    FOrganisationInfo Info;
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    UFUNCTION(BlueprintGetter)
    const FOrganisationInfo& GetInfo() const
    {
        return Info;
    }
    UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
    FORCEINLINE void SetInfo(const FOrganisationInfo& NewInfo)
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
