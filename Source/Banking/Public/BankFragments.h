#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "BankFragments.generated.h"

/**
 * Back-reference from a bank entity to its stable FBankId. Every accessor outside Mass addresses
 * a bank by this id, so each entity carries the id it was created for.
 */
USTRUCT()
struct BANKING_API FBankIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FBankId Id;
};

/**
 * Minimal on purpose, like FNationTag: banks carry no mutable simulation state yet. This tag is
 * the archetype anchor future gameplay fragments (interest rate, credit policy, ...) attach next
 * to, instead of retrofitting one later.
 */
USTRUCT()
struct BANKING_API FBankTag : public FMassTag
{
	GENERATED_BODY()
};
