#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BankInfo.generated.h"

/**
 * The UI-facing half of a bank: name and logo. Kept out of Mass for the same reason
 * FOrganizationInfo is — FText and FSlateBrush do not belong in a fragment. Lives in the
 * UBanking facade, indexed by the same int32 the bank entities carry.
 */
USTRUCT(BlueprintType)
struct BANKING_API FBankInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText BankName = FText::FromString("Unnamed Bank");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Logo = FSlateNoResource();
};
