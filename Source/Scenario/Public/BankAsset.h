// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BankAsset.generated.h"

/**
 * A human-authored bank, created as a Mass entity at scenario load. See UOrganizationAsset for
 * the same authoring pattern.
 */
UCLASS()
class SCENARIO_API UBankAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UBankAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText BankName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FSlateBrush Logo;
public:
	FORCEINLINE const FText& GetBankName() const { return BankName; }
	FORCEINLINE const FSlateBrush& GetLogo() const { return Logo; }
};

/**
 * Which bank account a character or organization should be opened with at scenario load, if any.
 * Bank == nullptr means no account — the same "unset reference means absent" convention
 * UCharacterAsset::Schedule already uses, no separate bool needed.
 */
USTRUCT(BlueprintType)
struct SCENARIO_API FBankAccountAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBankAsset* Bank = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InitialBalance = 0;
};
