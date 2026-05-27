// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameState.h"

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
	
	Time = CreateDefaultSubobject<UTime>(TEXT("Time"));
}

int32 ARtsGameState::GetTimezoneOffsetOfProvince(const int32 InProvinceIndex) const
{
	return ProvincesContainer->GetInfos()[InProvinceIndex].TimezoneOffset;
}
