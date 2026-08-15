#pragma once

#include "CoreMinimal.h"

/**
 * The population growth formula, kept free of Mass/world state so it can be called and tested in
 * isolation — see the Rts.GraphPopGrowth console command in the .cpp for how to get a curve out of
 * it without a running scenario.
 */
namespace PopulationGrowth
{
	/** Hours in a year, ignoring leap years — close enough for a growth curve. */
	POPULATIONS_API extern const float HoursPerYear;

	/**
	 * Compound growth: Size(t) = InStartSize * (1 + InAnnualBirthRate) ^ (InElapsedHours / HoursPerYear).
	 * Never returns a negative size, even for a declining (negative) rate.
	 */
	POPULATIONS_API int32 ComputeGrownSize(int32 InStartSize, float InAnnualBirthRate, float InElapsedHours);
}
