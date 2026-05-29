// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameModeBase.h"

#include "Characters.h"
#include "Nations.h"
#include "Provinces.h"
#include "Races.h"
#include "ScenarioLoading.h"
#include "RtsGameState.h"
#include "Scenario.h"
#include "Schedules.h"

ARtsGameModeBase::ARtsGameModeBase()
{
	GameStateClass = ARtsGameState::StaticClass();
}

void ARtsGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ScenarioToLoad == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid scenario"));
		return;
	}
	
	LoadScenario(ScenarioToLoad);
}

void ARtsGameModeBase::LoadScenario(UScenario* InScenario)
{
	ARtsGameState* RtsGameState = Cast<ARtsGameState>(GameState);
	if (RtsGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Game State"));
		return;
	}
	
	const auto[Races, RaceIndexes] = ScenarioLoader::ExtractRaces(InScenario->GetCharacters());
	RtsGameState->GetRacesContainer()->SetRaces(Races);

	const auto[Nations, Provinces] = ScenarioLoader::ExtractNationsAndProvinces(InScenario->GetProvinces());
	
	RtsGameState->GetNationsContainer()->SetInfos(Nations);
	RtsGameState->GetProvincesContainer()->Setup(Provinces);
	
	const auto[Infos, CurrentProvinceIndexes, ScheduleCharacterIndexMapping] = ScenarioLoader::ExtractCharacters(InScenario->GetCharacters(), InScenario->GetProvinces(), RaceIndexes);
	RtsGameState->GetCharactersContainer()->Setup(Infos, CurrentProvinceIndexes, ScheduleCharacterIndexMapping);

	// TODO: Somehow initialize schedules
	
	RtsGameState->GetBuildingsContainer()->InitWithProvinceAmount(Provinces.Num());
}

