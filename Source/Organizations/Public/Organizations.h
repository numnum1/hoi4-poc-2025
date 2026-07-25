#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "UObject/CoreNet.h"
#include "Organizations.generated.h"

class UOrganizationEntitySubsystem;

class FOrganizationsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrganizationsEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOrganizationEvent, int32, InOrganizationIndex);

/**
 * The UI-facing face of the organization data.
 *
 * Alignment and settled nations live in Mass entities owned by UOrganizationEntitySubsystem;
 * this component keeps only what does not belong in a fragment (names, logos) and the change
 * events the view models bind to. Every accessor takes the same int32 organization index as
 * before, so the bridge between Blueprint and Mass stays invisible to callers.
 */
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
    UPROPERTY(BlueprintAssignable)
    FOrganizationEvent OnAlignmentPublished;
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FOrganizationInfo> Infos;
public:
    UFUNCTION(BlueprintPure)
    int32 Num() const { return Infos.Num(); }

    UFUNCTION(BlueprintPure)
    FOrganizationInfo GetOrganizationInfo(int32 InIndex) const;

    // --- forwarded to the entity subsystem ---
    UFUNCTION(BlueprintPure)
    FOrganizationAlignment GetAlignment(int32 InOrganizationIndex) const;

    UFUNCTION(BlueprintPure)
    TArray<int32> GetSettledNations(int32 InOrganizationIndex) const;

    /** Founds an organization and returns its index. */
    UFUNCTION(BlueprintCallable)
    int32 Add(const FOrganizationInfo& NewOrg);

    UFUNCTION(BlueprintCallable)
    void PublishAlignment(int32 InOrganizationIndex, const TArray<float>& InAlignment);

    UFUNCTION(BlueprintCallable)
    void SettleNation(int32 InOrganizationIndex, int32 InNationIndex);

private:
    UOrganizationEntitySubsystem* GetEntities() const;
};
