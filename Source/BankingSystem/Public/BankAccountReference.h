// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BankAccountReference.generated.h"

class ABank;
/**
 * 
 */
USTRUCT(BlueprintType)
struct BANKINGSYSTEM_API FBankAccountReference
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABank* Bank = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid AccountID = FGuid();
};
