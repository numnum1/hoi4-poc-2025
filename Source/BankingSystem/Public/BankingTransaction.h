// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BankAccountReference.h"
#include "UObject/Object.h"
#include "BankingTransaction.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BANKINGSYSTEM_API FBankingTransaction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBankAccountReference Sender = {};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBankAccountReference Receiver = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Balance = 0;
	
};
