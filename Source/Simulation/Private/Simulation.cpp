#include "Simulation.h"

#include "time.h"

#define LOCTEXT_NAMESPACE "FSimulationModule"

void FSimulationModule::StartupModule()
{
    
}

void FSimulationModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSimulationModule, Simulation)

USimulation::USimulation()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void USimulation::SimulationTick(const float InDeltaHours)
{
	
}
