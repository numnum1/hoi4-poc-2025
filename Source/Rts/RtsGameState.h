// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buildings.h"
#include "OrganizationInfo.h"
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
	
	// Simulation
	FORCEINLINE USchedules* GetSchedules() const { return SchedulesContainer; }
	FORCEINLINE UTime* GetTime() const { return Time; }
	
	// Utils
	// Time
	UFUNCTION(BlueprintCallable)
	int32 GetTimezoneOffsetOfProvince(int32 InProvinceIndex) const;
	// Buildings
	UFUNCTION(BlueprintCallable)
	TArray<UBuildingType*> CollectBuildingsInProvince(int32 InProvinceIndex) const;
	UFUNCTION(BlueprintCallable)
	int32 CountBuildingByType(int32 InProvinceIndex, UBuildingType* InBuildingType) const;
	UFUNCTION(BlueprintCallable)
	int32 FindBuildingsBySlot(int32 InProvinceIndex, const FName& InSlotName) const;
	UFUNCTION(BlueprintCallable)
	void AddBuilding(UBuildingType* InBuilding, int32 InProvinceIndex, const FBuildingOwner& InBuildingOwner);
	// Characters
	UFUNCTION(BlueprintCallable)
	TArray<FOrganizationInfo> CollectOrganizationsForCharacter(int32 InCharacterIndex) const;
};
