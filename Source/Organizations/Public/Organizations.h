#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Organizations.generated.h"

class UOrganizationEntitySubsystem;

class FOrganizationsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrganizationsEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOrganizationEvent, FOrganizationId, InOrganizationId);

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
    /** Fired after an organization's entity is gone. Its display row (name, logo) stays around. */
    UPROPERTY(BlueprintAssignable)
    FOrganizationEvent OnOrganizationDissolved;
    /** Fired after SetRootDepartment pointed the organization at a different root department. */
    UPROPERTY(BlueprintAssignable)
    FOrganizationEvent OnRootDepartmentChanged;
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FOrganizationInfo> Infos;
public:
    /** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
    UFUNCTION(BlueprintPure)
    int32 Num() const { return Infos.Num(); }

    UFUNCTION(BlueprintPure)
    bool IsAlive(FOrganizationId InOrganizationId) const;

    UFUNCTION(BlueprintPure)
    FOrganizationInfo GetOrganizationInfo(FOrganizationId InOrganizationId) const;

    // --- forwarded to the entity subsystem ---
    /** The character the organization belongs to — its founder. Invalid for a scenario-authored one. */
    UFUNCTION(BlueprintPure)
    FCharacterId GetOwnerCharacter(FOrganizationId InOrganizationId) const;

    /** True if InCharacterId is the one character this organization belongs to. */
    UFUNCTION(BlueprintPure)
    bool IsOwnedBy(FOrganizationId InOrganizationId, FCharacterId InCharacterId) const;

    /** The department the organization is made of; UDepartments::GetSeat turns that into the
     * building it sits in. Invalid for a scenario-authored one, which is created without a
     * department — see FOrganizationRootDepartmentFragment. */
    UFUNCTION(BlueprintPure)
    FDepartmentId GetRootDepartment(FOrganizationId InOrganizationId) const;

    UFUNCTION(BlueprintPure)
    FOrganizationAlignment GetAlignment(FOrganizationId InOrganizationId) const;

    UFUNCTION(BlueprintPure)
    TArray<FNationId> GetSettledNations(FOrganizationId InOrganizationId) const;

    UFUNCTION(BlueprintPure)
    TArray<FOrganizationId> CollectOrganizationsSettledIn(FNationId InNationId) const;

    /** Founds an organization owned by InFounderId and returns its id. Pass an invalid founder for
     * an organization nobody founded — scenario-authored ones are loaded that way. */
    UFUNCTION(BlueprintCallable)
    FOrganizationId Add(const FOrganizationInfo& NewOrg, FCharacterId InFounderId);

    /** Points the organization at its root department. Called right after Add by whoever founded
     * it, once the department exists — the department names the organization, so the organization
     * has to be there first. */
    UFUNCTION(BlueprintCallable)
    void SetRootDepartment(FOrganizationId InOrganizationId, FDepartmentId InRootDepartmentId);

    UFUNCTION(BlueprintCallable)
    void PublishAlignment(FOrganizationId InOrganizationId, const TArray<float>& InAlignment);

    UFUNCTION(BlueprintCallable)
    void SettleNation(FOrganizationId InOrganizationId, FNationId InNationId);

    /** Destroys the entity. The display row (name, logo) stays for history. */
    UFUNCTION(BlueprintCallable)
    void Dissolve(FOrganizationId InOrganizationId);

private:
    UOrganizationEntitySubsystem* GetEntities() const;
};
