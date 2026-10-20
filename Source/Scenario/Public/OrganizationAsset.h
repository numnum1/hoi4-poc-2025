// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "BankAsset.h"
#include "Engine/DataAsset.h"
#include "OrganizationAsset.generated.h"

/**
 * A human-authored organization, founded at scenario load the same way a player-triggered
 * founding would be: ScenarioLoader converts this to an FOrganizationInfo and hands it to
 * UOrganizations::Add. See UProvinceAsset / UCharacterAsset for the same authoring pattern.
 */
UCLASS()
class SCENARIO_API UOrganizationAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UOrganizationAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText OrganizationName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Logo;
	/** Which bank account to open at scenario load. BankAccount.Bank == nullptr means none. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FBankAccountAsset BankAccount;
public:
	FORCEINLINE const FText& GetOrganizationName() const { return OrganizationName; }
	FORCEINLINE const FSlateBrush& GetLogo() const { return Logo; }
	FORCEINLINE const FBankAccountAsset& GetBankAccount() const { return BankAccount; }
};
