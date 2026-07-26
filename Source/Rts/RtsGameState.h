// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "RtsIds.h"
#include "GameFramework/GameStateBase.h"
#include "RtsGameState.generated.h"

class UOrganizations;
class UBuildings;
class UTime;
class URaces;
class UProvinces;
class UNations;
class UCharacters;
class USchedules;

/**
 * 
 */
UCLASS()
class RTS_API ARtsGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ARtsGameState();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UCharacters* CharactersContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UNations* NationsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UProvinces* ProvincesContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	URaces* RacesContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UBuildings* BuildingsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UOrganizations* OrganizationsContainer;
	
	// Simulation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	USchedules* SchedulesContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UTime* Time;
	
public:
	FORCEINLINE UCharacters* GetCharactersContainer() { return CharactersContainer; }
	FORCEINLINE UNations* GetNationsContainer() { return NationsContainer; }
	FORCEINLINE UProvinces* GetProvincesContainer() { return ProvincesContainer; }
	FORCEINLINE URaces* GetRacesContainer() { return RacesContainer; }
	FORCEINLINE UBuildings* GetBuildingsContainer() const { return BuildingsContainer; }
	FORCEINLINE UOrganizations* GetOrganizationsContainer() const { return OrganizationsContainer; }
	
	// Simulation
	FORCEINLINE USchedules* GetSchedules() const { return SchedulesContainer; }
	FORCEINLINE UTime* GetTime() const { return Time; }
	
	// Utils
	// Characters + Organizations: a genuine two-domain composition, so it stays here rather
	// than on either single-domain facade. Everything single-domain (timezone lookups, building
	// queries) moved to UProvinces/UBuildings directly — call those components, not the GameState.
	UFUNCTION(BlueprintCallable)
	TArray<FOrganizationInfo> CollectOrganizationsForCharacter(FCharacterId InCharacterId) const;
};
