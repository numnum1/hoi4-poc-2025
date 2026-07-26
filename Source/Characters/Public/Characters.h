#pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "CharacterInfo.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleInterface.h"
#include "Characters.generated.h"

class UCharacterEntitySubsystem;

class FCharactersModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

USTRUCT(BlueprintType)
struct CHARACTERS_API FPosition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FOrganizationId OrganizationId;
};

USTRUCT(BlueprintType)
struct CHARACTERS_API FPositions
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FPosition> Values = {};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharactersEvent, FCharacterId, InCharacterId);

/**
 * The UI-facing face of the character data.
 *
 * Simulation state lives in Mass entities owned by UCharacterEntitySubsystem; this component
 * keeps only what does not belong in a fragment (names, portraits) and the change events the
 * view models bind to. Every accessor takes the same int32 character index as before, so the
 * bridge between Blueprint and Mass stays invisible to callers.
 */
UCLASS()
class CHARACTERS_API UCharacters : public UActorComponent
{
    GENERATED_BODY()
public:
    UCharacters();

    UPROPERTY(BlueprintAssignable)
    FCharactersEvent OnCurrentProvinceChanged;
    UPROPERTY(BlueprintAssignable)
    FCharactersEvent OnScheduleChanged;
    UPROPERTY(BlueprintAssignable)
    FCharactersEvent OnPositionChanged;
    /** Fired once per character id after the hourly sim processor updates its vitals/skills. */
    UPROPERTY(BlueprintAssignable)
    FCharactersEvent OnSimulated;
    UPROPERTY(BlueprintAssignable)
    FCharactersEvent OnCharacterCreated;
    /** Fired after a character's entity is gone. Its display row (name, portrait) stays around. */
    UPROPERTY(BlueprintAssignable)
    FCharactersEvent OnCharacterDestroyed;

    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
private:
    /** One row per id ever created, alive or not — the dead keep their name and portrait. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FCharacterDisplayInfo> Display;
public:
    FORCEINLINE const TArray<FCharacterDisplayInfo>& GetDisplayInfos() const { return Display; }

    /** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
    UFUNCTION(BlueprintPure)
    int32 Num() const { return Display.Num(); }

    UFUNCTION(BlueprintPure)
    bool IsAlive(FCharacterId InCharacterId) const;

    UFUNCTION(BlueprintPure)
    FCharacterDisplayInfo GetDisplayInfo(FCharacterId InCharacterId) const;

    // --- forwarded to the entity subsystem ---
    UFUNCTION(BlueprintPure)
    FProvinceId GetCurrentProvince(FCharacterId InCharacterId) const;

    UFUNCTION(BlueprintPure)
    FAbilities GetAbilities(FCharacterId InCharacterId) const;

    UFUNCTION(BlueprintPure)
    TArray<FPosition> GetPositions(FCharacterId InCharacterId) const;

    UFUNCTION(BlueprintPure)
    FSchedule GetSchedule(FCharacterId InCharacterId) const;

    /** The USchedules library entry this character follows — pass it to USchedules to get its name. */
    UFUNCTION(BlueprintPure)
    FScheduleId GetScheduleId(FCharacterId InCharacterId) const;

    UFUNCTION(BlueprintPure)
    FCharacterVitals GetVitals(FCharacterId InCharacterId) const;

    UFUNCTION(BlueprintPure)
    FCharacterSkills GetSkills(FCharacterId InCharacterId) const;

    UFUNCTION(BlueprintPure)
    TArray<FCharacterId> CollectCharactersInProvince(FProvinceId InProvinceId) const;

    /** Creates the entity and its display row together and returns the id it claims. */
    UFUNCTION(BlueprintCallable)
    FCharacterId Create(const FCharacterSpawnData& InSpawnData);

    /** Destroys the entity. The display row (name, portrait) stays for genealogy/history. */
    UFUNCTION(BlueprintCallable)
    void Destroy(FCharacterId InCharacterId);

    UFUNCTION(BlueprintCallable)
    void AddPosition(FCharacterId InCharacterId, FOrganizationId InAtOrganizationId);

    UFUNCTION(BlueprintCallable)
    void MoveCharacter(FCharacterId InCharacterId, FProvinceId NewCurrentProvinceId);

private:
    UCharacterEntitySubsystem* GetEntities() const;

    void HandleCharacterSimulated(FCharacterId InCharacterId);
};
