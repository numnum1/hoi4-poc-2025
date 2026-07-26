#include "RtsProxySubsystem.h"

#include "BuildingProxy.h"
#include "Buildings.h"
#include "CharacterProxy.h"
#include "Characters.h"
#include "NationProxy.h"
#include "Nations.h"
#include "OrganizationProxy.h"
#include "Organizations.h"
#include "ProvinceProxy.h"
#include "Provinces.h"
#include "Roles.h"
#include "ScheduleProxy.h"
#include "Schedules.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

URtsProxySubsystem* URtsProxySubsystem::Get(const UObject* InWorldContextObject)
{
	if (InWorldContextObject == nullptr || GEngine == nullptr)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::ReturnNull);
	return World ? World->GetSubsystem<URtsProxySubsystem>() : nullptr;
}

template<typename ComponentType>
ComponentType* URtsProxySubsystem::ResolveContainer(TObjectPtr<ComponentType>& InOutCached)
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

template<typename ProxyType, typename IdType, typename ContainerType>
ProxyType* URtsProxySubsystem::GetOrCreateProxy(TMap<IdType, TObjectPtr<ProxyType>>& InCache, const IdType InId, ContainerType* InContainer)
{
	if (!InId.IsValid() || InContainer == nullptr)
	{
		return nullptr;
	}

	if (const TObjectPtr<ProxyType>* Found = InCache.Find(InId))
	{
		return Found->Get();
	}

	ProxyType* Proxy = NewObject<ProxyType>(this);
	Proxy->Setup(this, InId, InContainer);
	InCache.Add(InId, Proxy);
	return Proxy;
}

UCharacters* URtsProxySubsystem::GetCharactersContainer() { return ResolveContainer(CharactersContainer); }
UProvinces* URtsProxySubsystem::GetProvincesContainer() { return ResolveContainer(ProvincesContainer); }
UNations* URtsProxySubsystem::GetNationsContainer() { return ResolveContainer(NationsContainer); }
UOrganizations* URtsProxySubsystem::GetOrganizationsContainer() { return ResolveContainer(OrganizationsContainer); }
UBuildings* URtsProxySubsystem::GetBuildingsContainer() { return ResolveContainer(BuildingsContainer); }
USchedules* URtsProxySubsystem::GetSchedulesContainer() { return ResolveContainer(SchedulesContainer); }
URoles* URtsProxySubsystem::GetRolesContainer() { return ResolveContainer(RolesContainer); }

UCharacterProxy* URtsProxySubsystem::GetCharacter(const FCharacterId InCharacterId)
{
	return GetOrCreateProxy(CharacterProxies, InCharacterId, GetCharactersContainer());
}

UProvinceProxy* URtsProxySubsystem::GetProvince(const FProvinceId InProvinceId)
{
	return GetOrCreateProxy(ProvinceProxies, InProvinceId, GetProvincesContainer());
}

UNationProxy* URtsProxySubsystem::GetNation(const FNationId InNationId)
{
	return GetOrCreateProxy(NationProxies, InNationId, GetNationsContainer());
}

UOrganizationProxy* URtsProxySubsystem::GetOrganization(const FOrganizationId InOrganizationId)
{
	return GetOrCreateProxy(OrganizationProxies, InOrganizationId, GetOrganizationsContainer());
}

UBuildingProxy* URtsProxySubsystem::GetBuilding(const FBuildingId InBuildingId)
{
	return GetOrCreateProxy(BuildingProxies, InBuildingId, GetBuildingsContainer());
}

UScheduleProxy* URtsProxySubsystem::GetSchedule(const FScheduleId InScheduleId)
{
	return GetOrCreateProxy(ScheduleProxies, InScheduleId, GetSchedulesContainer());
}

TArray<UCharacterProxy*> URtsProxySubsystem::GetAllCharacters(const bool bAliveOnly)
{
	TArray<UCharacterProxy*> Result;
	UCharacters* Container = GetCharactersContainer();
	if (Container == nullptr)
	{
		return Result;
	}

	Result.Reserve(Container->Num());
	for (int32 Index = 0; Index < Container->Num(); ++Index)
	{
		const FCharacterId Id(Index);
		if (bAliveOnly && !Container->IsAlive(Id))
		{
			continue;
		}
		if (UCharacterProxy* Proxy = GetCharacter(Id))
		{
			Result.Add(Proxy);
		}
	}
	return Result;
}

TArray<UProvinceProxy*> URtsProxySubsystem::GetAllProvinces(const bool bAliveOnly)
{
	TArray<UProvinceProxy*> Result;
	UProvinces* Container = GetProvincesContainer();
	if (Container == nullptr)
	{
		return Result;
	}

	Result.Reserve(Container->Num());
	for (int32 Index = 0; Index < Container->Num(); ++Index)
	{
		const FProvinceId Id(Index);
		if (bAliveOnly && !Container->IsAlive(Id))
		{
			continue;
		}
		if (UProvinceProxy* Proxy = GetProvince(Id))
		{
			Result.Add(Proxy);
		}
	}
	return Result;
}

TArray<UNationProxy*> URtsProxySubsystem::GetAllNations(const bool bAliveOnly)
{
	TArray<UNationProxy*> Result;
	UNations* Container = GetNationsContainer();
	if (Container == nullptr)
	{
		return Result;
	}

	Result.Reserve(Container->Num());
	for (int32 Index = 0; Index < Container->Num(); ++Index)
	{
		const FNationId Id(Index);
		if (bAliveOnly && !Container->IsAlive(Id))
		{
			continue;
		}
		if (UNationProxy* Proxy = GetNation(Id))
		{
			Result.Add(Proxy);
		}
	}
	return Result;
}

TArray<UOrganizationProxy*> URtsProxySubsystem::GetAllOrganizations(const bool bAliveOnly)
{
	TArray<UOrganizationProxy*> Result;
	UOrganizations* Container = GetOrganizationsContainer();
	if (Container == nullptr)
	{
		return Result;
	}

	Result.Reserve(Container->Num());
	for (int32 Index = 0; Index < Container->Num(); ++Index)
	{
		const FOrganizationId Id(Index);
		if (bAliveOnly && !Container->IsAlive(Id))
		{
			continue;
		}
		if (UOrganizationProxy* Proxy = GetOrganization(Id))
		{
			Result.Add(Proxy);
		}
	}
	return Result;
}

TArray<UBuildingProxy*> URtsProxySubsystem::GetAllBuildings(const bool bAliveOnly)
{
	TArray<UBuildingProxy*> Result;
	UBuildings* Container = GetBuildingsContainer();
	if (Container == nullptr)
	{
		return Result;
	}

	Result.Reserve(Container->Num());
	for (int32 Index = 0; Index < Container->Num(); ++Index)
	{
		const FBuildingId Id(Index);
		if (bAliveOnly && !Container->IsAlive(Id))
		{
			continue;
		}
		if (UBuildingProxy* Proxy = GetBuilding(Id))
		{
			Result.Add(Proxy);
		}
	}
	return Result;
}

TArray<UScheduleProxy*> URtsProxySubsystem::GetAllSchedules()
{
	TArray<UScheduleProxy*> Result;
	USchedules* Container = GetSchedulesContainer();
	if (Container == nullptr)
	{
		return Result;
	}

	Result.Reserve(Container->Num());
	for (int32 Index = 0; Index < Container->Num(); ++Index)
	{
		if (UScheduleProxy* Proxy = GetSchedule(FScheduleId(Index)))
		{
			Result.Add(Proxy);
		}
	}
	return Result;
}

void URtsProxySubsystem::Deinitialize()
{
	// The facades go away with the world too, so drop the subscriptions before they do.
	auto UnbindAll = [](auto& InCache)
	{
		for (auto& Pair : InCache)
		{
			if (URtsEntityProxy* Proxy = Pair.Value)
			{
				Proxy->UnbindEvents();
			}
		}
		InCache.Empty();
	};

	UnbindAll(CharacterProxies);
	UnbindAll(ProvinceProxies);
	UnbindAll(NationProxies);
	UnbindAll(OrganizationProxies);
	UnbindAll(BuildingProxies);
	UnbindAll(ScheduleProxies);

	CharactersContainer = nullptr;
	ProvincesContainer = nullptr;
	NationsContainer = nullptr;
	OrganizationsContainer = nullptr;
	BuildingsContainer = nullptr;
	SchedulesContainer = nullptr;
	RolesContainer = nullptr;

	Super::Deinitialize();
}
