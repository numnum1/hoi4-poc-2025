#include "Race.h"

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