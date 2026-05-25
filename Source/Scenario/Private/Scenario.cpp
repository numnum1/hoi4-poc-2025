#include "Scenario.h"

#define LOCTEXT_NAMESPACE "FScenarioModule"

void FScenarioModule::StartupModule()
{
    
}

void FScenarioModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FScenarioModule, Scenario)

UScenario::UScenario()
{
	Provinces = {};
	Characters = {};
}