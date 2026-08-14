// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameState.h"

#include "Banking.h"
#include "Buildings.h"
#include "Characters.h"
#include "Departments.h"
#include "Economy/RtsEconomy.h"
#include "Nations.h"
#include "Organizations.h"
#include "Populations.h"
#include "Provinces.h"
#include "Races.h"
#include "Roles.h"
#include "Schedules.h"
#include "Time.h"

ARtsGameState::ARtsGameState()
{
	CharactersContainer = CreateDefaultSubobject<UCharacters>(TEXT("CharactersContainer"));
	NationsContainer = CreateDefaultSubobject<UNations>(TEXT("NationsContainer"));
	ProvincesContainer = CreateDefaultSubobject<UProvinces>(TEXT("ProvincesContainer"));
	PopulationsContainer = CreateDefaultSubobject<UPopulations>(TEXT("PopulationsContainer"));
	RacesContainer = CreateDefaultSubobject<URaces>(TEXT("RacesContainer"));
	BuildingsContainer = CreateDefaultSubobject<UBuildings>(TEXT("BuildingsContainer"));
	OrganizationsContainer = CreateDefaultSubobject<UOrganizations>(TEXT("OrganizationsContainer"));
	DepartmentsContainer = CreateDefaultSubobject<UDepartments>(TEXT("DepartmentsContainer"));
	RolesContainer = CreateDefaultSubobject<URoles>(TEXT("RolesContainer"));
	BankingContainer = CreateDefaultSubobject<UBanking>(TEXT("BankingContainer"));

	SchedulesContainer = CreateDefaultSubobject<USchedules>(TEXT("SchedulesContainer"));
	Time = CreateDefaultSubobject<UTime>(TEXT("Time"));
}

void ARtsGameState::BeginPlay()
{
	Super::BeginPlay();

	if (Time)
	{
		Time->OnHourElapsed.AddDynamic(this, &ARtsGameState::HandleHourElapsed);
	}
}

void ARtsGameState::HandleHourElapsed(const int32 InHoursElapsed)
{
	HoursIntoCurrentDay += InHoursElapsed;
	if (HoursIntoCurrentDay < 24)
	{
		return;
	}

	// At high Speed InHoursElapsed can cross more than one day boundary at once; settle once per
	// day crossed rather than losing the extra days silently.
	const int32 DaysElapsed = HoursIntoCurrentDay / 24;
	HoursIntoCurrentDay %= 24;

	FRtsEconomyContext Context;
	Context.Organizations = OrganizationsContainer;
	Context.Departments = DepartmentsContainer;
	Context.Characters = CharactersContainer;
	Context.Banking = BankingContainer;

	for (int32 Day = 0; Day < DaysElapsed; ++Day)
	{
		URtsEconomy::SettleDay(Context);
	}
}

TArray<FOrganizationInfo> ARtsGameState::CollectOrganizationsForCharacter(const FCharacterId InCharacterId) const
{
	TArray<FOrganizationInfo> Res;
	const TArray<FPosition> Positions = CharactersContainer->GetPositions(InCharacterId);
	Res.SetNum(Positions.Num());
	for (int32 I = 0; I < Positions.Num(); I++)
	{
		Res[I] = OrganizationsContainer->GetOrganizationInfo(Positions[I].OrganizationId);
	}
	return Res;
}