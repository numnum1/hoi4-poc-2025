#include "Race.h"

#include "Species.h"

#define LOCTEXT_NAMESPACE "FRaceModule"

void FRaceModule::StartupModule()
{
}

void FRaceModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRaceModule, Race)

URace::URace()
{
	Species = nullptr;
	RaceName = FText::FromString("Unnamed Race");
	BaseSkills = {};
	Traits = {};
}

TArray<URaceTrait*> URace::CollectTraits_Implementation() const
{
	TSet<URaceTrait*> Res = TSet(Traits);
	verify(Species);
	Res.Append(Species->CollectTraits_Implementation());
	return Res.Array();
}
