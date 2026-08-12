#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "RtsIds.h"
#include "Modules/ModuleManager.h"
#include "Provinces.generated.h"

class UProvinceEntitySubsystem;

class FProvincesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProvincesEvent, FProvinceId, InProvinceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProvincesSetupEvent);

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

    /** Fired once after Setup created every province of the scenario. Create() alone stays silent —
     * a listener that wants the finished map (ARtsWorldMap's default shading) would otherwise
     * rebuild itself once per province during scenario load. */
    UPROPERTY(BlueprintAssignable)
    FProvincesSetupEvent OnProvincesSetup;
    UPROPERTY(BlueprintAssignable)
    FProvincesEvent OnOwningNationChanged;
    /** Fired after a province's entity is gone. Its display row stays around. */
    UPROPERTY(BlueprintAssignable)
    FProvincesEvent OnProvinceDestroyed;
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TArray<FProvinceDisplayInfo> Display;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
    TMap<FName, FProvinceId> MapIds;

    /** Reverse of FProvinceDisplayInfo::MapPixelColor, keyed on the packed red/green channels —
     * see PackMapPixelColor for why the other two are dropped. */
    TMap<int32, FProvinceId> MapPixelColors;
public:
    /** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
    UFUNCTION(BlueprintPure)
    int32 Num() const { return Display.Num(); }

    UFUNCTION(BlueprintPure)
    bool IsAlive(FProvinceId InProvinceId) const;

    UFUNCTION(BlueprintPure)
    FProvinceDisplayInfo GetDisplayInfo(FProvinceId InProvinceId) const;

    UFUNCTION(BlueprintPure)
    FProvinceId FindByMapId(FName InMapId) const;

    /** This province's pixel color in the map's province-ID texture. Black for an unknown id, which
     * is also a legal authored color — check the id before trusting the result. */
    UFUNCTION(BlueprintPure)
    FColor GetMapPixelColor(FProvinceId InProvinceId) const;

    /** The province a province-ID pixel names. Only red and green are compared: the id lives in
     * those two channels, and a pixel sampled off the texture carries whatever the encoder left in
     * blue and alpha. Invalid when no province claims that color — a click on open water. */
    UFUNCTION(BlueprintPure)
    FProvinceId FindByMapPixelColor(FColor InMapPixelColor) const;

    UFUNCTION(BlueprintPure)
    FNationId GetOwningNation(FProvinceId InProvinceId) const;

    UFUNCTION(BlueprintPure)
    int32 GetTimezoneOffset(FProvinceId InProvinceId) const;

    UFUNCTION(BlueprintPure)
    TArray<FProvinceId> CollectProvincesOfNation(FNationId InNationId) const;

    /** Creates the entity and its display row together and returns the id it claims. */
    UFUNCTION(BlueprintCallable)
    FProvinceId Create(const FProvinceSpawnData& InSpawnData);

    /** Convenience for scenario load: creates one province per entry, in order. */
    UFUNCTION(BlueprintCallable)
    void Setup(const TArray<FProvinceSpawnData>& NewProvinces);

    UFUNCTION(BlueprintCallable)
    void SetOwningNation(FProvinceId InProvinceId, FNationId InNationId);

    /** Destroys the entity. The display row stays for history. */
    UFUNCTION(BlueprintCallable)
    void Destroy(FProvinceId InProvinceId);

private:
    UProvinceEntitySubsystem* GetEntities() const;
};
