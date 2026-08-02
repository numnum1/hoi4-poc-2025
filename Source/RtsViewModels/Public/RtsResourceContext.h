#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "RtsResourceContext.generated.h"

class UBanking;
class UCharacters;
class UOrganizations;

/**
 * Which kind of owner a resource context has. The value itself is never stored on anything
 * persistent — it only exists at this context/facade boundary, the same reason
 * EBuildingOwnerType is not stored on the entity either (see Buildings/Public/BuildingOwnerType.h).
 */
UENUM(BlueprintType)
enum class EResourceOwnerType : uint8
{
	ROT_None         UMETA(DisplayName = "None"),
	ROT_Character    UMETA(DisplayName = "Character"),
	ROT_Organization UMETA(DisplayName = "Organization"),
};

/**
 * Who owns the resources being asked about, plus the facades URtsResources reads through.
 *
 * Never assembled by hand: UPlayerCharacterViewModel builds it from the current selection, so
 * OwnerCharacterId/OwnerOrganizationId always come from a live position and cannot be mismatched.
 * The facades are carried instead of URtsResources resolving a subsystem itself, so every function
 * in URtsResources stays a pure function of its inputs — see FRtsActionContext for the same shape.
 */
USTRUCT(BlueprintType)
struct RTSVIEWMODELS_API FRtsResourceContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Resource")
	EResourceOwnerType OwnerType = EResourceOwnerType::ROT_None;

	/** Valid iff OwnerType == ROT_Character. */
	UPROPERTY(BlueprintReadOnly, Category="Resource")
	FCharacterId OwnerCharacterId;

	/** Valid iff OwnerType == ROT_Organization. */
	UPROPERTY(BlueprintReadOnly, Category="Resource")
	FOrganizationId OwnerOrganizationId;

	UPROPERTY(BlueprintReadOnly, Category="Resource")
	TObjectPtr<UCharacters> Characters;

	UPROPERTY(BlueprintReadOnly, Category="Resource")
	TObjectPtr<UOrganizations> Organizations;

	/** RT_Money reads its balance off this — see URtsResources::GetAmount. */
	UPROPERTY(BlueprintReadOnly, Category="Resource")
	TObjectPtr<UBanking> Banking;
};
