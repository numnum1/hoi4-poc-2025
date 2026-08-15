#include "PopGrowthProcessor.h"

#include "PopulationFragments.h"
#include "PopulationGrowth.h"
#include "MassExecutionContext.h"

UPopGrowthProcessor::UPopGrowthProcessor()
	: SimQuery(*this)
{
	// UPopulationEntitySubsystem runs this off UTime::OnHourElapsed, not off a frame phase.
	bAutoRegisterWithProcessingPhases = false;
	bRequiresGameThreadExecution = true;
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Standalone | EProcessorExecutionFlags::Server);
}

void UPopGrowthProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	SimQuery.AddTagRequirement<FPopTag>(EMassFragmentPresence::All);
	SimQuery.AddConstSharedRequirement<FPopGrowthSharedFragment>(EMassFragmentPresence::All);
	SimQuery.AddRequirement<FPopSizeFragment>(EMassFragmentAccess::ReadWrite);
}

void UPopGrowthProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	if (ElapsedHours <= 0.f)
	{
		return;
	}

	const float LocalElapsedHours = ElapsedHours;

	SimQuery.ForEachEntityChunk(Context, [LocalElapsedHours](FMassExecutionContext& Ctx)
	{
		// Shared per chunk, so fetch it once rather than per entity.
		const float AnnualBirthRate = Ctx.GetConstSharedFragment<FPopGrowthSharedFragment>().AnnualBirthRate;

		const TArrayView<FPopSizeFragment> Sizes = Ctx.GetMutableFragmentView<FPopSizeFragment>();

		const int32 NumEntities = Ctx.GetNumEntities();
		for (int32 EntityIt = 0; EntityIt < NumEntities; ++EntityIt)
		{
			Sizes[EntityIt].Size = PopulationGrowth::ComputeGrownSize(Sizes[EntityIt].Size, AnnualBirthRate, LocalElapsedHours);
		}
	});
}
