#pragma once

#include "CoreMinimal.h"
#include "EthnicityInfo.h"
#include "PopulationInfo.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Populations.generated.h"

class UPopulationEntitySubsystem;

class FPopulationsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPopulationsSetupEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPopulationsEvent, FPopId, InPopId);

/**
 * The UI-facing face of the population data.
 *
 * The people live in Mass entities owned by UPopulationEntitySubsystem — one per province and
 * ethnicity. This component keeps only what does not belong in a fragment: the authored ethnicity
 * library (names and map colors, never mutated) plus the change events view models bind to.
 *
 * It knows nothing about provinces beyond their ids, which is why this module does not depend on
 * Provinces — a caller that wants a province's name asks UProvinces for it.
 */
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class POPULATIONS_API UPopulations : public UActorComponent
{
	GENERATED_BODY()

public:
	UPopulations();

	/** Fired once after Setup created every pop of the scenario. Create() alone stays silent — a
	 * listener that wants the finished world would otherwise rebuild itself once per pop during
	 * scenario load, the same reason UProvinces::OnProvincesSetup exists. */
	UPROPERTY(BlueprintAssignable)
	FPopulationsSetupEvent OnPopulationsSetup;

	/** Fired after a pop's head count changed. Carries the pop, not the province — a listener that
	 * cares about a province resolves it through GetPopInfo. */
	UPROPERTY(BlueprintAssignable)
	FPopulationsEvent OnPopSizeChanged;

	UPROPERTY(BlueprintAssignable)
	FPopulationsEvent OnPopDestroyed;

	//~ Ethnicity library ---------------------------------------------------------------------------

	/** Replaces the ethnicity library. Scenario load only — an FEthnicityId is a position in here,
	 * so changing it after pops exist would repoint every one of them. */
	UFUNCTION(BlueprintCallable)
	void SetupEthnicities(const TArray<FEthnicityInfo>& InEthnicities);

	UFUNCTION(BlueprintPure)
	int32 NumEthnicities() const { return Ethnicities.Num(); }

	UFUNCTION(BlueprintPure)
	FEthnicityInfo GetEthnicityInfo(FEthnicityId InEthnicityId) const;

	//~ Pops ----------------------------------------------------------------------------------------

	/** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
	UFUNCTION(BlueprintPure)
	int32 Num() const;

	UFUNCTION(BlueprintPure)
	bool IsAlive(FPopId InPopId) const;

	UFUNCTION(BlueprintPure)
	FPopInfo GetPopInfo(FPopId InPopId) const;

	/** Every living pop in one pass — what a whole-world reader (the map's ethnicity mode) should
	 * use instead of asking province by province. */
	UFUNCTION(BlueprintPure)
	TArray<FPopInfo> CollectPops() const;

	UFUNCTION(BlueprintPure)
	TArray<FPopInfo> CollectPopsInProvince(FProvinceId InProvinceId) const;

	UFUNCTION(BlueprintPure)
	int32 GetProvincePopulation(FProvinceId InProvinceId) const;

	/** The ethnicity most people in the province belong to, or an invalid id if nobody lives there.
	 * Ties go to the lower id so the map does not flicker between two equal groups. */
	UFUNCTION(BlueprintPure)
	FEthnicityId GetDominantEthnicity(FProvinceId InProvinceId) const;

	/** Creates the entity and returns the id it claims. */
	UFUNCTION(BlueprintCallable)
	FPopId Create(const FPopSpawnData& InSpawnData);

	/** Convenience for scenario load: creates one pop per entry, in order. */
	UFUNCTION(BlueprintCallable)
	void Setup(const TArray<FPopSpawnData>& InSpawnData);

	UFUNCTION(BlueprintCallable)
	void SetSize(FPopId InPopId, int32 InSize);

	UFUNCTION(BlueprintCallable)
	void Destroy(FPopId InPopId);

private:
	/** One row per FEthnicityId, authored at scenario load. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FEthnicityInfo> Ethnicities;

	UPopulationEntitySubsystem* GetEntities() const;
};
