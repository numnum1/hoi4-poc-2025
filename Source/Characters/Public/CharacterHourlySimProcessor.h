#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "CharacterHourlySimProcessor.generated.h"

/**
 * Advances every simulated character through the hours that just elapsed, following its
 * schedule. Deliberately not registered with a processing phase: UCharacterEntitySubsystem
 * runs it by hand once per crossed game hour, so results are identical whether the game ran
 * at Speed 1 or Speed 1000.
 */
UCLASS()
class CHARACTERS_API UCharacterHourlySimProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UCharacterHourlySimProcessor();

	/** InStartHour is the first hour to simulate; InHours is how many to walk from there. */
	void SetStep(uint8 InStartHour, int32 InHours)
	{
		StartHour = InStartHour;
		HoursToSim = InHours;
	}

	/** Character ids touched by the most recent Execute. UCharacterEntitySubsystem fans this out as UI hooks. */
	const TArray<FCharacterId>& GetLastSimulatedIds() const { return LastSimulatedIds; }

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery SimQuery;

	uint8 StartHour = 0;
	int32 HoursToSim = 0;

	TArray<FCharacterId> LastSimulatedIds;
};
