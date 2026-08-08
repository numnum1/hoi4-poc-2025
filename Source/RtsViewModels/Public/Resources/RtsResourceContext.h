#pragma once

#include "CoreMinimal.h"
#include "Resources/ResourceOwner.h"
#include "RtsResourceContext.generated.h"

class UBanking;
class UCharacters;
class UDepartments;
class UOrganizations;

/**
 * Who owns the resources being asked about, plus the facades URtsResources reads through.
 *
 * Never assembled by hand: UPlayerCharacterViewModel builds it from the current selection, so the
 * owner always comes from a live position. The facades are carried instead of URtsResources
 * resolving a subsystem itself, so every function in URtsResources stays a pure function of its
 * inputs — see FRtsActionContext for the same shape.
 */
USTRUCT(BlueprintType)
struct RTSVIEWMODELS_API FRtsResourceContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Resource")
	FResourceOwner Owner;

	UPROPERTY(BlueprintReadOnly, Category="Resource")
	TObjectPtr<UCharacters> Characters;

	UPROPERTY(BlueprintReadOnly, Category="Resource")
	TObjectPtr<UOrganizations> Organizations;

	/** RT_Money reads its balance off this — see URtsResources::GetAmount. */
	UPROPERTY(BlueprintReadOnly, Category="Resource")
	TObjectPtr<UBanking> Banking;

	/** RT_Manpower reads capacity (URtsPresence::GetCapacity) off this — see URtsResources::GetAmount. */
	UPROPERTY(BlueprintReadOnly, Category="Resource")
	TObjectPtr<UDepartments> Departments;
};
