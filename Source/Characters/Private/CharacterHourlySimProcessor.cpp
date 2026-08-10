#include "CharacterHourlySimProcessor.h"

#include "CharacterFragments.h"
#include "MassExecutionContext.h"

UCharacterHourlySimProcessor::UCharacterHourlySimProcessor()
	: SimQuery(*this)
{
	// UCharacterEntitySubsystem runs this off UTime::OnHourElapsed, not off a frame phase.
	bAutoRegisterWithProcessingPhases = false;
	bRequiresGameThreadExecution = true;
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Standalone | EProcessorExecutionFlags::Server);
}

void UCharacterHourlySimProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	SimQuery.AddTagRequirement<FCharacterTag>(EMassFragmentPresence::All);
	// Normal or Max, but not None — CST_None characters carry neither tag and are skipped.
	SimQuery.AddTagRequirement<FCharacterSimNormalTag>(EMassFragmentPresence::Any);
	SimQuery.AddTagRequirement<FCharacterSimMaxTag>(EMassFragmentPresence::Any);

	SimQuery.AddConstSharedRequirement<FCharacterScheduleSharedFragment>(EMassFragmentPresence::All);
	SimQuery.AddRequirement<FCharacterIndexFragment>(EMassFragmentAccess::ReadOnly);
	SimQuery.AddRequirement<FCharacterLocationFragment>(EMassFragmentAccess::ReadOnly);
	SimQuery.AddRequirement<FCharacterVitalsFragment>(EMassFragmentAccess::ReadWrite);
	SimQuery.AddRequirement<FCharacterSkillsFragment>(EMassFragmentAccess::ReadWrite);
}

void UCharacterHourlySimProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	LastSimulatedIds.Reset();

	if (HoursToSim <= 0)
	{
		return;
	}

	const uint8 LocalStartHour = StartHour;
	const int32 LocalHours = HoursToSim;

	SimQuery.ForEachEntityChunk(Context, [this, LocalStartHour, LocalHours](FMassExecutionContext& Ctx)
	{
		// Shared per chunk, so fetch it once rather than per entity.
		const FSchedule& Schedule = Ctx.GetConstSharedFragment<FCharacterScheduleSharedFragment>().Schedule;

		const TConstArrayView<FCharacterIndexFragment> Indices = Ctx.GetFragmentView<FCharacterIndexFragment>();
		const TConstArrayView<FCharacterLocationFragment> Locations = Ctx.GetFragmentView<FCharacterLocationFragment>();
		const TArrayView<FCharacterVitalsFragment> Vitals = Ctx.GetMutableFragmentView<FCharacterVitalsFragment>();
		const TArrayView<FCharacterSkillsFragment> Skills = Ctx.GetMutableFragmentView<FCharacterSkillsFragment>();

		const int32 NumEntities = Ctx.GetNumEntities();
		for (int32 EntityIt = 0; EntityIt < NumEntities; ++EntityIt)
		{
			LastSimulatedIds.Add(Indices[EntityIt].Id);

			const int32 TimezoneOffset = Locations[EntityIt].TimezoneOffset;
			FCharacterVitalsFragment& V = Vitals[EntityIt];
			FCharacterSkillsFragment& S = Skills[EntityIt];

			for (int32 HourIt = 0; HourIt < LocalHours; ++HourIt)
			{
				// Hour 14 means different things in different provinces.
				const int32 Shifted = LocalStartHour + HourIt + TimezoneOffset;
				const uint8 LocalHour = static_cast<uint8>(((Shifted % 24) + 24) % 24);

				switch (Schedule[LocalHour])
				{
				case EScheduleEntryType::SET_SLEEP:
					V.Energy += 4;
					V.Happiness += 1;
					break;
				case EScheduleEntryType::SET_WORK:
					V.Energy -= 3;
					V.Happiness -= 2;
					break;
				case EScheduleEntryType::SET_LEISURE:
					V.Energy -= 1;
					V.Happiness += 4;
					break;
				case EScheduleEntryType::SET_STUDY:
					V.Energy -= 2;
					V.Happiness -= 1;
					S.Science += 1;
					S.Strength += 1;
					S.Medicine += 1;
					break;
				case EScheduleEntryType::SET_NONE:
				default:
					// An unauthored schedule is all NONE, so staying silent here is deliberate.
					break;
				}
			}

			V.Energy = FMath::Clamp(V.Energy, 0, 100);
			V.Happiness = FMath::Clamp(V.Happiness, 0, 100);
		}
	});
}
