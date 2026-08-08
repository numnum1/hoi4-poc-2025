#pragma once

#include "CoreMinimal.h"
#include "RtsEconomyContext.generated.h"

class UBanking;
class UCharacters;
class UDepartments;
class UOrganizations;

/**
 * The facades URtsEconomy::SettleDay reads and mutates through, carried in rather than resolved by
 * URtsEconomy itself — same shape as FRtsActionContext/FRtsResourceContext. ARtsGameState builds
 * this from the components it already owns and calls SettleDay once per elapsed game day.
 */
USTRUCT(BlueprintType)
struct RTSVIEWMODELS_API FRtsEconomyContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Economy")
	TObjectPtr<UOrganizations> Organizations;

	UPROPERTY(BlueprintReadOnly, Category="Economy")
	TObjectPtr<UDepartments> Departments;

	UPROPERTY(BlueprintReadOnly, Category="Economy")
	TObjectPtr<UCharacters> Characters;

	UPROPERTY(BlueprintReadOnly, Category="Economy")
	TObjectPtr<UBanking> Banking;
};
