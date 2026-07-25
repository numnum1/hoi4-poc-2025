#include "Time.h"

#include "GameFramework/Actor.h"
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
	LastHourBoundary = TruncateToHour(CurrentTime);
}

FDateTime UTime::TruncateToHour(const FDateTime& In)
{
	return FDateTime(In.GetYear(), In.GetMonth(), In.GetDay(), In.GetHour());
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
	// CurrentTime is replicated, so only the server may advance it. Without this the client
	// integrates locally and fights every incoming OnRep_CurrentTime.
	if (GetOwner() && !GetOwner()->HasAuthority()) return;

	CurrentTime += FTimespan::FromSeconds(DeltaTime * 60 * 60 * Speed);
	OnCurrentTimeChanged.Broadcast();

	const FDateTime NewBoundary = TruncateToHour(CurrentTime);
	if (NewBoundary <= LastHourBoundary) return;

	int32 HoursElapsed = FMath::RoundToInt((NewBoundary - LastHourBoundary).GetTotalHours());
	LastHourBoundary = NewBoundary;

	// At extreme Speed a single tick can cover an absurd span. Simulating it would hitch,
	// so cap the catch-up and say so rather than silently dropping time.
	static constexpr int32 MaxCatchUpHours = 24 * 7;
	if (HoursElapsed > MaxCatchUpHours)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTime: %d game hours elapsed in one tick, clamping to %d"),
			HoursElapsed, MaxCatchUpHours);
		HoursElapsed = MaxCatchUpHours;
	}

	OnHourElapsed.Broadcast(HoursElapsed);
}
