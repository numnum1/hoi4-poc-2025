#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RtsFacadeSubsystem.generated.h"

class UBanking;
class UBuildings;
class UCharacters;
class UNations;
class UOrganizations;
class UProvinces;
class USchedules;
class URoles;
class UTime;

/**
 * Resolves the facade components off the current world's game state and caches them.
 *
 * There used to be a per-entity proxy layer here too; it is gone (see CLAUDE.md) — view models
 * now read the facades below directly. What is left is exactly the part no view model can do for
 * itself: finding ARtsGameState and pulling its components off, once, per world.
 *
 * Asking before the game state exists returns nullptr; nothing is cached against a nullptr
 * component, so the next call after the game state comes up succeeds.
 */
UCLASS()
class RTSVIEWMODELS_API URtsFacadeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** The registry for the world InWorldContextObject belongs to, or nullptr outside a world. */
	UFUNCTION(BlueprintPure, Category="Rts Facades", meta=(WorldContext="InWorldContextObject", DisplayName="Get Rts Facades"))
	static URtsFacadeSubsystem* Get(const UObject* InWorldContextObject);

	//~ The facades ------------------------------------------------------------------------------
	// Resolved from the game state on first use.

	UCharacters* GetCharactersContainer();
	UProvinces* GetProvincesContainer();
	UNations* GetNationsContainer();
	UOrganizations* GetOrganizationsContainer();
	UBuildings* GetBuildingsContainer();
	USchedules* GetSchedulesContainer();
	URoles* GetRolesContainer();
	UBanking* GetBankingContainer();
	/** Not a domain facade, but the same kind of game state component: the clock. */
	UTime* GetTimeContainer();

	//~ Begin USubsystem interface
	virtual void Deinitialize() override;
	//~ End USubsystem interface

private:
	/** The game state component of type T, or nullptr while the game state is still coming up. */
	template<typename ComponentType>
	ComponentType* ResolveContainer(TObjectPtr<ComponentType>& InOutCached);

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

	UPROPERTY()
	TObjectPtr<UBanking> BankingContainer;

	UPROPERTY()
	TObjectPtr<UTime> TimeContainer;
};
