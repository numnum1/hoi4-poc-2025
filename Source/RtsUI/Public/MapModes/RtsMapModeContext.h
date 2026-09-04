#pragma once

#include "CoreMinimal.h"
#include "RtsMapModeContext.generated.h"

class UBuildings;
class UCharacters;
class UDepartments;
class UNations;
class UOrganizations;
class UPopulations;
class UProvinces;

/**
 * The facades a map mode reads to color the world.
 *
 * Carried in rather than resolved inside URtsMapModes, the same shape FRtsActionContext uses, so
 * every function there stays a pure function of its inputs. URtsHUDViewModel builds it once from
 * the facades it already holds.
 *
 * Not every mode needs every facade — Political only reads Provinces and Nations. A mode that finds
 * one of the facades it needs missing returns nothing at all rather than a half-painted map.
 */
USTRUCT(BlueprintType)
struct RTSUI_API FRtsMapModeContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Map Mode")
	TObjectPtr<UProvinces> Provinces;

	UPROPERTY(BlueprintReadOnly, Category="Map Mode")
	TObjectPtr<UNations> Nations;

	UPROPERTY(BlueprintReadOnly, Category="Map Mode")
	TObjectPtr<UOrganizations> Organizations;

	/** An organization's headquarters is its root department's seat, which is a building, which
	 * sits in a province — three facades for one lookup, which is why this mode lives in RtsUI. */
	UPROPERTY(BlueprintReadOnly, Category="Map Mode")
	TObjectPtr<UDepartments> Departments;

	UPROPERTY(BlueprintReadOnly, Category="Map Mode")
	TObjectPtr<UBuildings> Buildings;

	UPROPERTY(BlueprintReadOnly, Category="Map Mode")
	TObjectPtr<UPopulations> Populations;

	UPROPERTY(BlueprintReadOnly, Category="Map Mode")
	TObjectPtr<UCharacters> Characters;
};
