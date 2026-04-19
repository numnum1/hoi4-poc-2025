#include "TimeSimulation.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FTimeSimulationModule"

void FTimeSimulationModule::StartupModule()
{
}

void FTimeSimulationModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTimeSimulationModule, TimeSimulation)

ATimeSimulation::ATimeSimulation()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	PrimaryActorTick.TickInterval = 1.f;
	DateTime = FDateTime{2026, 4, 19, 13, 0, 0, 0};
}

void ATimeSimulation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATimeSimulation, DateTime);
}

void ATimeSimulation::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		const auto Timespan = UKismetMathLibrary::MakeTimespan(0, 0, 0, DeltaSeconds * 3600, 0);
		DateTime += Timespan;

		OnDateTimeChanged.Broadcast();
	}
}
