#include "PopulationGrowth.h"

DEFINE_LOG_CATEGORY_STATIC(LogRtsPopulationGrowth, Log, All);

namespace PopulationGrowth
{
	const float HoursPerYear = 24.f * 365.f;

	int32 ComputeGrownSize(const int32 InStartSize, const float InAnnualBirthRate, const float InElapsedHours)
	{
		const float Years = InElapsedHours / HoursPerYear;
		const float Grown = static_cast<float>(InStartSize) * FMath::Pow(1.f + InAnnualBirthRate, Years);
		return FMath::Max(0, FMath::RoundToInt32(Grown));
	}
}

namespace
{
	/**
	 * Prints a Year,Size series to the log so it can be pasted into a spreadsheet and graphed —
	 * exercises PopulationGrowth::ComputeGrownSize without needing a scenario or a running world.
	 * Usage: Rts.GraphPopGrowth <StartSize> <AnnualRatePercent> <Years>
	 */
	void GraphPopGrowth(const TArray<FString>& Args)
	{
		if (Args.Num() < 3)
		{
			UE_LOG(LogRtsPopulationGrowth, Warning, TEXT("Usage: Rts.GraphPopGrowth <StartSize> <AnnualRatePercent> <Years>"));
			return;
		}

		const int32 StartSize = FCString::Atoi(*Args[0]);
		const float AnnualBirthRate = FCString::Atof(*Args[1]) / 100.f;
		const int32 Years = FCString::Atoi(*Args[2]);

		UE_LOG(LogRtsPopulationGrowth, Log, TEXT("Year,Size"));
		for (int32 Year = 0; Year <= Years; ++Year)
		{
			const int32 Size = PopulationGrowth::ComputeGrownSize(StartSize, AnnualBirthRate, Year * PopulationGrowth::HoursPerYear);
			UE_LOG(LogRtsPopulationGrowth, Log, TEXT("%d,%d"), Year, Size);
		}
	}

	static FAutoConsoleCommand GGraphPopGrowthCommand(
		TEXT("Rts.GraphPopGrowth"),
		TEXT("Logs a Year,Size series for a given start size and annual birth rate percent. Usage: Rts.GraphPopGrowth <StartSize> <AnnualRatePercent> <Years>"),
		FConsoleCommandWithArgsDelegate::CreateStatic(&GraphPopGrowth));
}
