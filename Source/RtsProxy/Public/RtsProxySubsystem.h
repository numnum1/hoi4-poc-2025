#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Subsystems/WorldSubsystem.h"
#include "RtsProxySubsystem.generated.h"

class UBuildingProxy;
class UBuildings;
class UCharacterProxy;
class UCharacters;
class UNationProxy;
class UNations;
class UOrganizationProxy;
class UOrganizations;
class UProvinceProxy;
class UProvinces;
class UScheduleProxy;
class USchedules;
class URoles;

/**
 * Hands out the entity proxies and owns them.
 *
 * There is exactly one proxy per id, created on first request and kept for the lifetime of the
 * world, so a proxy pointer can be stored in a variable and compared by identity. Handing them
 * out from one place is also what lets a proxy return proxies of other domains — this subsystem
 * is the only object in the project that knows every facade at once, which is why it and the
 * proxies live in their own module.
 *
 * Asking for an invalid id, or asking before the game state exists, returns nullptr rather than
 * a placeholder object: Blueprint chains degrade to an access-none warning, and C++ callers get
 * the null check the compiler expects. Nothing is cached against a nullptr container, so the
 * next call after the game state comes up succeeds.
 */
UCLASS()
class RTSPROXY_API URtsProxySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** The registry for the world InWorldContextObject belongs to, or nullptr outside a world. */
	UFUNCTION(BlueprintPure, Category="Rts Proxies", meta=(WorldContext="InWorldContextObject", DisplayName="Get Rts Proxies"))
	static URtsProxySubsystem* Get(const UObject* InWorldContextObject);

	//~ One proxy per id -------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	UCharacterProxy* GetCharacter(FCharacterId InCharacterId);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	UProvinceProxy* GetProvince(FProvinceId InProvinceId);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	UNationProxy* GetNation(FNationId InNationId);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	UOrganizationProxy* GetOrganization(FOrganizationId InOrganizationId);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	UBuildingProxy* GetBuilding(FBuildingId InBuildingId);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	UScheduleProxy* GetSchedule(FScheduleId InScheduleId);

	//~ Everything of a kind ---------------------------------------------------------------------
	// bAliveOnly skips ids whose entity is gone. Pass false to walk history as well — the display
	// rows of destroyed characters, provinces and nations are deliberately kept by their facades.

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	TArray<UCharacterProxy*> GetAllCharacters(bool bAliveOnly = true);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	TArray<UProvinceProxy*> GetAllProvinces(bool bAliveOnly = true);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	TArray<UNationProxy*> GetAllNations(bool bAliveOnly = true);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	TArray<UOrganizationProxy*> GetAllOrganizations(bool bAliveOnly = true);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	TArray<UBuildingProxy*> GetAllBuildings(bool bAliveOnly = true);

	UFUNCTION(BlueprintPure, Category="Rts Proxies")
	TArray<UScheduleProxy*> GetAllSchedules();

	//~ The facades ------------------------------------------------------------------------------
	// Resolved from the game state on first use. The proxies read through these; call them
	// directly only for the handful of operations no proxy covers (creating entities, say).

	UCharacters* GetCharactersContainer();
	UProvinces* GetProvincesContainer();
	UNations* GetNationsContainer();
	UOrganizations* GetOrganizationsContainer();
	UBuildings* GetBuildingsContainer();
	USchedules* GetSchedulesContainer();
	URoles* GetRolesContainer();

	//~ Begin USubsystem interface
	virtual void Deinitialize() override;
	//~ End USubsystem interface

private:
	/** Find-or-create against one of the caches below. Defined in the .cpp, one use per domain. */
	template<typename ProxyType, typename IdType, typename ContainerType>
	ProxyType* GetOrCreateProxy(TMap<IdType, TObjectPtr<ProxyType>>& InCache, IdType InId, ContainerType* InContainer);

	/** The game state component of type T, or nullptr while the game state is still coming up. */
	template<typename ComponentType>
	ComponentType* ResolveContainer(TObjectPtr<ComponentType>& InOutCached);

	UPROPERTY()
	TMap<FCharacterId, TObjectPtr<UCharacterProxy>> CharacterProxies;

	UPROPERTY()
	TMap<FProvinceId, TObjectPtr<UProvinceProxy>> ProvinceProxies;

	UPROPERTY()
	TMap<FNationId, TObjectPtr<UNationProxy>> NationProxies;

	UPROPERTY()
	TMap<FOrganizationId, TObjectPtr<UOrganizationProxy>> OrganizationProxies;

	UPROPERTY()
	TMap<FBuildingId, TObjectPtr<UBuildingProxy>> BuildingProxies;

	UPROPERTY()
	TMap<FScheduleId, TObjectPtr<UScheduleProxy>> ScheduleProxies;

	UPROPERTY()
	TObjectPtr<UCharacters> CharactersContainer;

	UPROPERTY()
	TObjectPtr<UProvinces> ProvincesContainer;

	UPROPERTY()
	TObjectPtr<UNations> NationsContainer;

	UPROPERTY()
	TObjectPtr<UOrganizations> OrganizationsContainer;

	UPROPERTY()
	TObjectPtr<UBuildings> BuildingsContainer;

	UPROPERTY()
	TObjectPtr<USchedules> SchedulesContainer;

	UPROPERTY()
	TObjectPtr<URoles> RolesContainer;
};
