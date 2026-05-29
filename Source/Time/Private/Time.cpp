#include "Time.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FTimeModule"

void FTimeModule::StartupModule()
{
    
}

void FTimeModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FTimeModule, Time)

UTime::UTime()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.1f;
	
	SetIsReplicatedByDefault(true);
	
	CurrentTime = FDateTime(2026, 5, 27, 11, 20, 0, 0);
	Speed = 1;
}

void UTime::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UTime, CurrentTime);
}

void UTime::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateTime(DeltaTime);
}

void UTime::UpdateTime(const float DeltaTime)
{
	if (Speed == 0) return;
	check(DeltaTime >= 0);
	
	CurrentTime += FTimespan::FromSeconds(DeltaTime * 60 * 60 * Speed);
	OnCurrentTimeChanged.Broadcast();
}
