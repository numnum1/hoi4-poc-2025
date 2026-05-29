#include "Schedules.h"

#define LOCTEXT_NAMESPACE "FSchedulesModule"

void FSchedulesModule::StartupModule()
{
    
}

void FSchedulesModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSchedulesModule, Schedules)

USchedules::USchedules()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}