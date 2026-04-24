// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BankAccount.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, BlueprintType)
class BANKINGSYSTEM_API UBankAccount : public UObject
{
	GENERATED_BODY()
public:
	UBankAccount();
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	FGuid ID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	int32 Balance;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FORCEINLINE virtual bool IsSupportedForNetworking() const override { return true; }
	
	FORCEINLINE const FGuid& GetID() const { return ID; }
	FORCEINLINE const int32& GetBalance() const { return Balance; }
};
