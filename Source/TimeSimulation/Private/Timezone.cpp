// Fill out your copyright notice in the Description page of Project Settings.


#include "Timezone.h"
#include "TimeSimulation.h"

ATimezone::ATimezone()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Offset = 0;
}

void ATimezone::BeginPlay()
{
	Super::BeginPlay();

	verifyf(TimeSimulation, TEXT("Missing Time Simulation Actor"));
	TimeSimulation->OnDateTimeChanged.AddDynamic(this, &ATimezone::ATimezone::Update);
	
	ActualDateTime = TimeSimulation->GetDateTime() + FTimespan(Offset, 0, 0);
}

void ATimezone::Update()
{
	const FDateTime& DateTime = TimeSimulation->GetDateTime();
	const FDateTime NewDateTime = DateTime + FTimespan(Offset, 0, 0);
	const FTimespan Diff = NewDateTime - ActualDateTime;
	ActualDateTime = NewDateTime;
	
	int32 PassedDays = Diff.GetDays();
	int32 PassedHours = Diff.GetHours();
	
	if (PassedDays > 0) OnDaysPassed.Broadcast(PassedDays);
	if (PassedHours > 0) OnHoursPassed.Broadcast(PassedHours);
}
