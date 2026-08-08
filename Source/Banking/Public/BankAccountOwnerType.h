#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BankAccountOwnerType.generated.h"

/**
 * Which kind of owner a bank account has. The value itself is never stored on the entity — it is
 * read back from which owner fragment is present (FBankAccountCharacterOwnerFragment xor
 * FBankAccountOrganizationOwnerFragment), so this enum exists only at the Blueprint/facade
 * boundary.
 */
UENUM(BlueprintType)
enum class EBankAccountOwnerType : uint8
{
	/** No living owner fragment, e.g. the account id itself is not alive. */
	BAOT_None         UMETA(DisplayName = "None"),
	BAOT_Character    UMETA(DisplayName = "Character"),
	BAOT_Organization UMETA(DisplayName = "Organization")
};
