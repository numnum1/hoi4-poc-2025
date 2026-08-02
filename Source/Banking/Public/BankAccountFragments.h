#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "BankAccountFragments.generated.h"

/**
 * Back-reference from an account entity to its stable FBankAccountId.
 */
USTRUCT()
struct BANKING_API FBankAccountIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FBankAccountId Id;
};

/** Which bank this account is held at. Every account has exactly one, unlike ownership below,
 * which is either/or — so this is its own fragment rather than folded into an owner fragment. */
USTRUCT()
struct BANKING_API FBankAccountBankFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FBankId BankId;
};

USTRUCT()
struct BANKING_API FBankAccountBalanceFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Balance = 0;
};

/**
 * Ownership is not a stored enum: exactly one of FBankAccountCharacterOwnerFragment /
 * FBankAccountOrganizationOwnerFragment is present on an entity, and
 * UBankAccountEntitySubsystem::GetOwnerType reads that back from fragment presence — the same
 * pattern as FBuildingCharacterOwnerFragment / FBuildingOrganizationOwnerFragment.
 */
USTRUCT()
struct BANKING_API FBankAccountCharacterOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FCharacterId OwnerId;
};

USTRUCT()
struct BANKING_API FBankAccountOrganizationOwnerFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FOrganizationId OwnerId;
};

USTRUCT()
struct BANKING_API FBankAccountTag : public FMassTag
{
	GENERATED_BODY()
};
