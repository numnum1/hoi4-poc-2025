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
	RaceName = FText::FromString("Unnamed Race");
	Category = nullptr;
	BaseSkills = {};
}