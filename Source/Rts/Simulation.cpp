#include "Simulation.h"

#include "Schedule.h"
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

struct CharacterSimulationData { int32 Energy; int32 Happiness; };
struct CharacterSkills { int32 Science; int32 Strength; int32 Medicine; };

void USimulation::SimulationTick(const float InDeltaHours)
{
	// Testing code
	uint8 DeltaHours = 2;
	uint8 LastHour = 4;
	TArray<FSchedule> Schedules = {};
	TArray<int32> CharactersToSchedules = {};
	TArray<CharacterSimulationData> CharacterData = {}; 
	TArray<CharacterSkills> Skills = {};
	// Actual function
	for (int I = 0; I < CharactersToSchedules.Num(); ++I)
	{
		const FSchedule& Schedule = Schedules[CharactersToSchedules[I]];
		for (uint8 J = 0; J < 24; J += DeltaHours)
		{
			const uint8 CurrentHour = (LastHour + J) % 24;
			const EScheduleEntryType Entry = Schedule[CurrentHour];
			auto& [Energy, Happiness] = CharacterData[I];
			auto & [Science, Strength, Medicine] = Skills[I];
			switch (Entry)
			{
				case EScheduleEntryType::SET_NONE:
					UE_LOG(LogTemp, Warning, TEXT("Invalid Entry"));
				case EScheduleEntryType::SET_LEISURE:
					Energy += InDeltaHours * -1;
					Happiness += InDeltaHours * 4;
				case EScheduleEntryType::SET_STUDY:
					Energy += InDeltaHours * -2;
					Happiness += InDeltaHours * -1;
					Science += InDeltaHours * 1;
					Strength += InDeltaHours * 1;
					Medicine += InDeltaHours * 1;
			case EScheduleEntryType::SET_SLEEP:
					Energy += InDeltaHours * 4;
					Happiness += InDeltaHours * 1;
				case EScheduleEntryType::SET_WORK:
					Energy += InDeltaHours * -3;
					Happiness += InDeltaHours * -2;
			}
		}
	}
}
