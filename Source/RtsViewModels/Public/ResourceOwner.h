#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "ResourceOwner.generated.h"

/**
 * Which kind of legal person owns resources. The value is never stored on anything persistent —
 * it exists only at this context/facade boundary, the same reason EBankAccountOwnerType is not
 * stored on the entity either (see Banking/Public/BankAccountOwnerType.h).
 */
UENUM(BlueprintType)
enum class EResourceOwnerType : uint8
{
	ROT_None         UMETA(DisplayName = "None"),
	ROT_Character    UMETA(DisplayName = "Character"),
	ROT_Organization UMETA(DisplayName = "Organization"),
};

/**
 * One legal person — a character or an organization — as a single parameter type: the id plus
 * which kind of id it is.
 *
 * It exists because "a character or an organization" is a recurring parameter across this system
 * (which resources to show, what a balance reads as) and passing two ids plus a discriminator by
 * hand invites the mismatch this struct makes impossible: build one through ForCharacter/
 * ForOrganization and the type and the id can never disagree. Deliberately not an interface or a
 * base class with two subclasses — there are exactly two cases, forever, and CLAUDE.md's
 * enum + switch rule covers exactly this.
 */
USTRUCT(BlueprintType)
struct RTSVIEWMODELS_API FResourceOwner
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Resource")
	EResourceOwnerType OwnerType = EResourceOwnerType::ROT_None;

	/** Valid iff OwnerType == ROT_Character. */
	UPROPERTY(BlueprintReadOnly, Category="Resource")
	FCharacterId CharacterId;

	/** Valid iff OwnerType == ROT_Organization. */
	UPROPERTY(BlueprintReadOnly, Category="Resource")
	FOrganizationId OrganizationId;

	static FResourceOwner ForCharacter(const FCharacterId InCharacterId)
	{
		FResourceOwner Owner;
		Owner.OwnerType = EResourceOwnerType::ROT_Character;
		Owner.CharacterId = InCharacterId;
		return Owner;
	}

	static FResourceOwner ForOrganization(const FOrganizationId InOrganizationId)
	{
		FResourceOwner Owner;
		Owner.OwnerType = EResourceOwnerType::ROT_Organization;
		Owner.OrganizationId = InOrganizationId;
		return Owner;
	}
};
