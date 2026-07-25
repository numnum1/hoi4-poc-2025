#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "Modules/ModuleManager.h"
#include "Provinces.generated.h"

class UProvinceEntitySubsystem;

class FProvincesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

/**
 * The UI-facing face of the province data.
 *
 * Ownership lives in Mass entities owned by UProvinceEntitySubsystem; this component keeps
 * only what does not belong in a fragment (name, picture, sound, ecology, timezone offset —
 * all authored once and never mutated) plus the MapId lookup. Every accessor takes the same
 * int32 province index as before, so the bridge between Blueprint and Mass stays invisible.
 */
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class PROVINCES_API UProvinces : public UActorComponent
{
    GENERATED_BODY()
public:
    UProvinces();
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FProvinceDisplayInfo> Display;
    // Optimization
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TMap<FName, int32> MapIds;
public:
    UFUNCTION(BlueprintPure)
    int32 Num() const { return Display.Num(); }

    UFUNCTION(BlueprintPure)
    FProvinceDisplayInfo GetDisplayInfo(int32 InProvinceIndex) const;

    UFUNCTION(BlueprintPure)
    int32 FindByMapId(FName InMapId) const;

    // --- forwarded to the entity subsystem ---
    UFUNCTION(BlueprintPure)
    int32 GetOwningNation(int32 InProvinceIndex) const;

    UFUNCTION(BlueprintPure)
    TArray<int32> CollectProvincesOfNation(int32 InNationIndex) const;

    /** Fills the display rows and spawns the province entities. */
    UFUNCTION(BlueprintCallable)
    void Setup(const TArray<FProvinceSpawnData>& NewProvinces);

    UFUNCTION(BlueprintCallable)
    void SetOwningNation(int32 InProvinceIndex, int32 InNationIndex);

private:
    UProvinceEntitySubsystem* GetEntities() const;
};
