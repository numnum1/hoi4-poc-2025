#pragma once

#include "CoreMinimal.h"
#include "NationInfo.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Nations.generated.h"

class UNationEntitySubsystem;

class FNationsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNationsEvent, FNationId, InNationId);

/**
 * The UI-facing face of the nation data.
 *
 * Identity lives in Mass entities owned by UNationEntitySubsystem; this component keeps only
 * what does not belong in a fragment (name, flag) and the change events the view models bind
 * to. Every accessor takes the same FNationId every other nation API uses, so the bridge
 * between Blueprint and Mass stays invisible to callers.
 */
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class NATIONS_API UNations : public UActorComponent
{
    GENERATED_BODY()
public:
    UNations();

    /** Fired after a nation's entity is gone. Its display row (name, flag) stays around. */
    UPROPERTY(BlueprintAssignable)
    FNationsEvent OnNationDestroyed;
private:
    /** One row per id ever created, alive or not — the dead keep their name and flag. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FNationInfo> Infos;
public:
    /** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
    UFUNCTION(BlueprintPure)
    int32 Num() const { return Infos.Num(); }

    UFUNCTION(BlueprintPure)
    bool IsAlive(FNationId InNationId) const;

    UFUNCTION(BlueprintPure)
    FNationInfo GetNationInfo(FNationId InNationId) const;

    /** Creates the entity and its display row together and returns the id it claims. */
    UFUNCTION(BlueprintCallable)
    FNationId Create(const FNationInfo& NewInfo);

    /** Convenience for scenario load: creates one nation per entry, in order. */
    UFUNCTION(BlueprintCallable)
    void Setup(const TArray<FNationInfo>& NewInfos);

    /** Destroys the entity. The display row (name, flag) stays for history. */
    UFUNCTION(BlueprintCallable)
    void Destroy(FNationId InNationId);

private:
    UNationEntitySubsystem* GetEntities() const;
};
