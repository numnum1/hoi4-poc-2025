#pragma once

#include "CoreMinimal.h"
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "PopGrowthProcessor.generated.h"

/**
 * Grows every pop by the elapsed time since the last run, using its ethnicity's annual birth rate.
 * Unlike UCharacterHourlySimProcessor, growth is a closed-form formula (see PopulationGrowth.h), so
 * there is no per-hour loop here — the whole elapsed span is applied in one calculation per entity.
 *
 * Deliberately not registered with a processing phase: UPopulationEntitySubsystem runs it by hand
 * once per crossed game hour, the same way UCharacterEntitySubsystem drives its hourly sim.
 */
UCLASS()
class POPULATIONS_API UPopGrowthProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UPopGrowthProcessor();

	void SetElapsedHours(const float InElapsedHours) { ElapsedHours = InElapsedHours; }

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery SimQuery;

	float ElapsedHours = 0.f;
};
