// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameModeBase.h"

#include "RtsGameState.h"
#include "Scenario.h"

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
	
	InScenario->GetCharacters();
	InScenario->GetProvinces();
}

