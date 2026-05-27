// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameState.h"

#include "BuildingType.h"
#include "Time.h"
#include "Characters.h"
#include "Nations.h"
#include "Provinces.h"
#include "Races.h"

ARtsGameState::ARtsGameState()
{
	CharactersContainer = CreateDefaultSubobject<UCharacters>(TEXT("CharactersContainer"));
	NationsContainer = CreateDefaultSubobject<UNations>(TEXT("NationsContainer"));
	ProvincesContainer = CreateDefaultSubobject<UProvinces>(TEXT("ProvincesContainer"));
	RacesContainer = CreateDefaultSubobject<URaces>(TEXT("RacesContainer"));
	BuildingsContainer = CreateDefaultSubobject<UBuildings>(TEXT("BuildingsContainer"));
	
	Time = CreateDefaultSubobject<UTime>(TEXT("Time"));
}

int32 ARtsGameState::GetTimezoneOffsetOfProvince(const int32 InProvinceIndex) const
{
	return ProvincesContainer->GetInfos()[InProvinceIndex].TimezoneOffset;
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
