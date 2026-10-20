// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "RtsIds.h"
#include "GameFramework/GameStateBase.h"
#include "RtsGameState.generated.h"

class UOrganizations;
class UDepartments;
class UBanking;
class UBuildings;
class UTime;
class UProvinces;
class UPopulations;
class UNations;
class UCharacters;
class USchedules;
class URoles;
class UOrganizationFocuses;

/**
 * 
 */
UCLASS()
class RTS_API ARtsGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ARtsGameState();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UCharacters* CharactersContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UNations* NationsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UProvinces* ProvincesContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UPopulations* PopulationsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UBuildings* BuildingsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UOrganizations* OrganizationsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UDepartments* DepartmentsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	URoles* RolesContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UBanking* BankingContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UOrganizationFocuses* OrganizationFocusesContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	USchedules* SchedulesContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UTime* Time;

	/** Accumulates Time->OnHourElapsed until a full day has passed, then runs URtsEconomy::SettleDay
	 * — the same role UCharacterEntitySubsystem::BindToTime plays for the hourly character sim, one
	 * level up because this crosses domains (Departments/Characters/Banking) and so cannot live in
	 * any single domain module. */
	UFUNCTION()
	void HandleHourElapsed(int32 InHoursElapsed);

	int32 HoursIntoCurrentDay = 0;

public:
	FORCEINLINE UCharacters* GetCharactersContainer() { return CharactersContainer; }
	FORCEINLINE UNations* GetNationsContainer() { return NationsContainer; }
	FORCEINLINE UProvinces* GetProvincesContainer() { return ProvincesContainer; }
	FORCEINLINE UPopulations* GetPopulationsContainer() const { return PopulationsContainer; }
	FORCEINLINE UBuildings* GetBuildingsContainer() const { return BuildingsContainer; }
	FORCEINLINE UOrganizations* GetOrganizationsContainer() const { return OrganizationsContainer; }
	FORCEINLINE UDepartments* GetDepartmentsContainer() const { return DepartmentsContainer; }
	FORCEINLINE URoles* GetRoles() const { return RolesContainer; }
	FORCEINLINE UBanking* GetBankingContainer() const { return BankingContainer; }
	FORCEINLINE UOrganizationFocuses* GetOrganizationFocusesContainer() const { return OrganizationFocusesContainer; }

	FORCEINLINE USchedules* GetSchedules() const { return SchedulesContainer; }
	FORCEINLINE UTime* GetTime() const { return Time; }

	// Characters + Organizations: a genuine two-domain composition, so it stays here rather
	// than on either single-domain facade. Everything single-domain (timezone lookups, building
	// queries) moved to UProvinces/UBuildings directly — call those components, not the GameState.
	UFUNCTION(BlueprintCallable)
	TArray<FOrganizationInfo> CollectOrganizationsForCharacter(FCharacterId InCharacterId) const;
};
