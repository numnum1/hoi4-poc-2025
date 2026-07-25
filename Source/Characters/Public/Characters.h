#pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "CharacterInfo.h"
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
    int32 OrganizationIndex = 0;
};

USTRUCT(BlueprintType)
struct CHARACTERS_API FPositions
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FPosition> Values = {};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharactersEvent, int32, InCharacterIndex);

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
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FCharacterDisplayInfo> Display;
public:
    FORCEINLINE const TArray<FCharacterDisplayInfo>& GetDisplayInfos() const { return Display; }

    UFUNCTION(BlueprintPure)
    int32 Num() const { return Display.Num(); }

    UFUNCTION(BlueprintPure)
    FCharacterDisplayInfo GetDisplayInfo(int32 InCharacterIndex) const;

    // --- forwarded to the entity subsystem ---
    UFUNCTION(BlueprintPure)
    int32 GetCurrentProvince(int32 InCharacterIndex) const;

    UFUNCTION(BlueprintPure)
    FAbilities GetAbilities(int32 InCharacterIndex) const;

    UFUNCTION(BlueprintPure)
    TArray<FPosition> GetPositions(int32 InCharacterIndex) const;

    UFUNCTION(BlueprintPure)
    FSchedule GetSchedule(int32 InCharacterIndex) const;

    /** Fills the display rows. Entity creation is UCharacterEntitySubsystem::CreateCharacters. */
    UFUNCTION(BlueprintCallable)
    void SetupDisplay(const TArray<FCharacterSpawnData>& InSpawnData);

    UFUNCTION(BlueprintCallable)
    void AddPosition(int32 InCharacterIndex, int32 InAtOrganizationIndex);

    /**
     * NewTimezoneOffset is cached on the entity so the hourly processor needs no province
     * lookup; resolve it with ARtsGameState::GetTimezoneOffsetOfProvince.
     */
    UFUNCTION(BlueprintCallable)
    void MoveCharacter(int32 InCharacterIndex, int32 NewCurrentProvinceIndex, int32 NewTimezoneOffset);

private:
    UCharacterEntitySubsystem* GetEntities() const;
};
