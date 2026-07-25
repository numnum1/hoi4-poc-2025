// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameModeBase.h"

#include "CharacterEntitySubsystem.h"
#include "Characters.h"
#include "Nations.h"
#include "Provinces.h"
#include "Races.h"
#include "ScenarioLoading.h"
#include "RtsGameState.h"
#include "Scenario.h"
#include "Schedules.h"
#include "Time.h"

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
	
	const auto [Categories, LifeTypes, Species, Races, RaceIndexes] = ScenarioLoader::ExtractRaces(InScenario->GetCharacters());
	URaces* RacesContainer = RtsGameState->GetRacesContainer();
	RacesContainer->SetCategories(Categories);
	RacesContainer->SetLifeTypes(LifeTypes);
	RacesContainer->SetSpecies(Species);
	RacesContainer->SetRaces(Races);

	const auto[Nations, Provinces] = ScenarioLoader::ExtractNationsAndProvinces(InScenario->GetProvinces());
	
	RtsGameState->GetNationsContainer()->SetInfos(Nations);
	RtsGameState->GetProvincesContainer()->Setup(Provinces);
	
	const TArray<FCharacterSpawnData> CharacterSpawnData = ScenarioLoader::ExtractCharacters(
		InScenario->GetCharacters(), InScenario->GetProvinces(), Provinces, RaceIndexes, Races);

	UCharacterEntitySubsystem* CharacterEntities = GetWorld()->GetSubsystem<UCharacterEntitySubsystem>();
	if (CharacterEntities == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No UCharacterEntitySubsystem — characters will not be simulated"));
		return;
	}
	CharacterEntities->CreateCharacters(CharacterSpawnData);
	CharacterEntities->BindToTime(RtsGameState->GetTime());

	RtsGameState->GetCharactersContainer()->SetupDisplay(CharacterSpawnData);

	// TODO: Somehow initialize schedules
	
	RtsGameState->GetBuildingsContainer()->InitWithProvinceAmount(Provinces.Num());
}

