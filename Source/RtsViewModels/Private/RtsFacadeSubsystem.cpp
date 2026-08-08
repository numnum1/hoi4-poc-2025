#include "RtsFacadeSubsystem.h"

#include "Banking.h"
#include "Buildings.h"
#include "Characters.h"
#include "Departments.h"
#include "Nations.h"
#include "Organizations.h"
#include "Provinces.h"
#include "Roles.h"
#include "Schedules.h"
#include "Time.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

URtsFacadeSubsystem* URtsFacadeSubsystem::Get(const UObject* InWorldContextObject)
{
	if (InWorldContextObject == nullptr || GEngine == nullptr)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::ReturnNull);
	return World ? World->GetSubsystem<URtsFacadeSubsystem>() : nullptr;
}

template<typename ComponentType>
ComponentType* URtsFacadeSubsystem::ResolveContainer(TObjectPtr<ComponentType>& InOutCached)
{
	if (InOutCached != nullptr)
	{
		return InOutCached;
	}

	if (const UWorld* World = GetWorld())
	{
		if (const AGameStateBase* GameState = World->GetGameState())
		{
			InOutCached = GameState->FindComponentByClass<ComponentType>();
		}
	}

	return InOutCached;
}

UCharacters* URtsFacadeSubsystem::GetCharactersContainer() { return ResolveContainer(CharactersContainer); }
UProvinces* URtsFacadeSubsystem::GetProvincesContainer() { return ResolveContainer(ProvincesContainer); }
UNations* URtsFacadeSubsystem::GetNationsContainer() { return ResolveContainer(NationsContainer); }
UOrganizations* URtsFacadeSubsystem::GetOrganizationsContainer() { return ResolveContainer(OrganizationsContainer); }
UDepartments* URtsFacadeSubsystem::GetDepartmentsContainer() { return ResolveContainer(DepartmentsContainer); }
UBuildings* URtsFacadeSubsystem::GetBuildingsContainer() { return ResolveContainer(BuildingsContainer); }
USchedules* URtsFacadeSubsystem::GetSchedulesContainer() { return ResolveContainer(SchedulesContainer); }
URoles* URtsFacadeSubsystem::GetRolesContainer() { return ResolveContainer(RolesContainer); }
UBanking* URtsFacadeSubsystem::GetBankingContainer() { return ResolveContainer(BankingContainer); }
UTime* URtsFacadeSubsystem::GetTimeContainer() { return ResolveContainer(TimeContainer); }

void URtsFacadeSubsystem::Deinitialize()
{
	CharactersContainer = nullptr;
	ProvincesContainer = nullptr;
	NationsContainer = nullptr;
	OrganizationsContainer = nullptr;
	DepartmentsContainer = nullptr;
	BuildingsContainer = nullptr;
	SchedulesContainer = nullptr;
	RolesContainer = nullptr;
	BankingContainer = nullptr;
	TimeContainer = nullptr;

	Super::Deinitialize();
}
