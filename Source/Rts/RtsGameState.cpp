// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameState.h"

#include "BuildingType.h"
#include "Time.h"
#include "Characters.h"
#include "Nations.h"
#include "Organizations.h"
#include "Provinces.h"
#include "Races.h"
#include "Schedules.h"

ARtsGameState::ARtsGameState()
{
	CharactersContainer = CreateDefaultSubobject<UCharacters>(TEXT("CharactersContainer"));
	NationsContainer = CreateDefaultSubobject<UNations>(TEXT("NationsContainer"));
	ProvincesContainer = CreateDefaultSubobject<UProvinces>(TEXT("ProvincesContainer"));
	RacesContainer = CreateDefaultSubobject<URaces>(TEXT("RacesContainer"));
	BuildingsContainer = CreateDefaultSubobject<UBuildings>(TEXT("BuildingsContainer"));
	OrganizationsContainer = CreateDefaultSubobject<UOrganizations>(TEXT("OrganizationsContainer"));
	
	SchedulesContainer = CreateDefaultSubobject<USchedules>(TEXT("SchedulesContainer"));
	Time = CreateDefaultSubobject<UTime>(TEXT("Time"));
}

int32 ARtsGameState::GetTimezoneOffsetOfProvince(const int32 InProvinceIndex) const
{
	return ProvincesContainer->GetDisplayInfo(InProvinceIndex).TimezoneOffset;
}

TArray<UBuildingType*> ARtsGameState::CollectBuildingsInProvince(const int32 InProvinceIndex) const
{
	const int32 Num = BuildingsContainer->GetBuildingCounts()[InProvinceIndex];
	TArray<UBuildingType*> Buildings;
	Buildings.SetNum(Num);
	for (int32 I = 0; I < Num; I++)
	{
		Buildings[I] = BuildingsContainer->GetBuildingPool()[InProvinceIndex * MAX_BUILDINGS_PER_PROVINCE + I];
	}
	return Buildings;
}

int32 ARtsGameState::CountBuildingByType(const int32 InProvinceIndex, UBuildingType* InBuildingType) const
{
	int32 Res = 0;
	for (int32 I = 0; I < BuildingsContainer->GetBuildingCounts()[InProvinceIndex]; I++)
	{
		const auto Type = BuildingsContainer->GetBuildingPool()[InProvinceIndex * MAX_BUILDINGS_PER_PROVINCE + I];
		if (Type == InBuildingType)
		{
			Res++;
		}
	}
	return Res;
}

int32 ARtsGameState::FindBuildingsBySlot(const int32 InProvinceIndex, const FName& InSlotName) const
{
	int32 Res = 0;
	for (int32 I = 0; I < BuildingsContainer->GetBuildingCounts()[InProvinceIndex]; I++)
	{
		const auto Type = BuildingsContainer->GetBuildingPool()[InProvinceIndex * MAX_BUILDINGS_PER_PROVINCE + I];
		if (Type->GetBuildingSlotName() == InSlotName)
		{
			Res++;
		}
	}
	return Res;
}

void ARtsGameState::AddBuilding(UBuildingType* InBuilding, const int32 InProvinceIndex, const FBuildingOwner& InBuildingOwner)
{
	BuildingsContainer->Add(InBuilding, InProvinceIndex, InBuildingOwner);
}

TArray<FOrganizationInfo> ARtsGameState::CollectOrganizationsForCharacter(const int32 InCharacterIndex) const
{
	TArray<FOrganizationInfo> Res;
	const TArray<FPosition> Positions = CharactersContainer->GetPositions(InCharacterIndex);
	Res.SetNum(Positions.Num());
	for (int32 I = 0; I < Positions.Num(); I++)
	{
		Res[I] = OrganizationsContainer->GetOrganizationInfo(Positions[I].OrganizationIndex);
	}
	return Res;
}