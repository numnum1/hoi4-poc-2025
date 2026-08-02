// Fill out your copyright notice in the Description page of Project Settings.


#include "RtsGameModeBase.h"

#include "Banking.h"
#include "CharacterEntitySubsystem.h"
#include "Characters.h"
#include "Nations.h"
#include "Organizations.h"
#include "Provinces.h"
#include "Races.h"
#include "ScenarioLoading.h"
#include "Roles.h"
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

	RtsGameState->GetNationsContainer()->Setup(Nations);
	RtsGameState->GetProvincesContainer()->Setup(Provinces);

	// Banks first: independent of everything else, and characters/organizations below may open an
	// account with one, so the FBankId their FBankAccountAsset resolves to must already exist.
	auto [BankInfos, BankAssets] = ScenarioLoader::ExtractBanks(InScenario->GetBanks());
	UBanking* BankingContainer = RtsGameState->GetBankingContainer();
	for (const FBankInfo& BankInfo : BankInfos)
	{
		BankingContainer->AddBank(BankInfo);
	}

	// Schedules first: characters resolve their FScheduleId against the library's asset order.
	auto [ScheduleInfos, ScheduleAssets] = ScenarioLoader::ExtractSchedules(InScenario->GetCharacters());
	RtsGameState->GetSchedules()->Setup(MoveTemp(ScheduleInfos));

	auto [CharacterSpawnData, CharacterAssets] = ScenarioLoader::ExtractCharacters(
		InScenario->GetCharacters(), InScenario->GetProvinces(), Provinces, RaceIndexes, Races, ScheduleAssets);

	UCharacterEntitySubsystem* CharacterEntities = GetWorld()->GetSubsystem<UCharacterEntitySubsystem>();
	if (CharacterEntities == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No UCharacterEntitySubsystem — characters will not be simulated"));
		return;
	}
	CharacterEntities->BindToTime(RtsGameState->GetTime());

	UCharacters* CharactersContainer = RtsGameState->GetCharactersContainer();
	for (int32 Index = 0; Index < CharacterSpawnData.Num(); ++Index)
	{
		CharactersContainer->Create(CharacterSpawnData[Index]);

		const FBankAccountAsset& BankAccount = CharacterAssets[Index]->GetBankAccount();
		if (BankAccount.Bank != nullptr)
		{
			const FBankId BankId(BankAssets.Find(BankAccount.Bank));
			const FBankAccountId AccountId = BankingContainer->OpenAccountForCharacter(BankId, FCharacterId(Index));
			BankingContainer->Deposit(AccountId, BankAccount.InitialBalance);
		}
	}

	// Roles before organizations: UActionFoundOrganization resolves a leadership role for the
	// founder's position, so the library must already be filled by the time an org can be created.
	RtsGameState->GetRoles()->Setup(ScenarioLoader::ExtractRoles(InScenario->GetRoles()));

	auto [OrganizationInfos, OrganizationAssets] = ScenarioLoader::ExtractOrganizations(InScenario->GetOrganizations());

	UOrganizations* OrganizationsContainer = RtsGameState->GetOrganizationsContainer();
	for (int32 Index = 0; Index < OrganizationInfos.Num(); ++Index)
	{
		// Nobody founded these — they exist from the start, so they start out ownerless.
		OrganizationsContainer->Add(OrganizationInfos[Index], FCharacterId());

		const FBankAccountAsset& BankAccount = OrganizationAssets[Index]->GetBankAccount();
		if (BankAccount.Bank != nullptr)
		{
			const FBankId BankId(BankAssets.Find(BankAccount.Bank));
			const FBankAccountId AccountId = BankingContainer->OpenAccountForOrganization(BankId, FOrganizationId(Index));
			BankingContainer->Deposit(AccountId, BankAccount.InitialBalance);
		}
	}
}

