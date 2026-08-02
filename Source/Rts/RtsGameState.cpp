// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameState.h"

#include "Banking.h"
#include "Buildings.h"
#include "Time.h"
#include "Characters.h"
#include "Nations.h"
#include "Organizations.h"
#include "Provinces.h"
#include "Races.h"
#include "Roles.h"
#include "Schedules.h"

ARtsGameState::ARtsGameState()
{
	CharactersContainer = CreateDefaultSubobject<UCharacters>(TEXT("CharactersContainer"));
	NationsContainer = CreateDefaultSubobject<UNations>(TEXT("NationsContainer"));
	ProvincesContainer = CreateDefaultSubobject<UProvinces>(TEXT("ProvincesContainer"));
	RacesContainer = CreateDefaultSubobject<URaces>(TEXT("RacesContainer"));
	BuildingsContainer = CreateDefaultSubobject<UBuildings>(TEXT("BuildingsContainer"));
	OrganizationsContainer = CreateDefaultSubobject<UOrganizations>(TEXT("OrganizationsContainer"));
	RolesContainer = CreateDefaultSubobject<URoles>(TEXT("RolesContainer"));
	BankingContainer = CreateDefaultSubobject<UBanking>(TEXT("BankingContainer"));

	SchedulesContainer = CreateDefaultSubobject<USchedules>(TEXT("SchedulesContainer"));
	Time = CreateDefaultSubobject<UTime>(TEXT("Time"));
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