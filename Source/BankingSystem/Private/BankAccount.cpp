// Fill out your copyright notice in the Description page of Project Settings.


#include "BankAccount.h"

#include "Net/UnrealNetwork.h"

UBankAccount::UBankAccount()
{
	ID = FGuid();
	Balance = 0;
}

void UBankAccount::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UBankAccount, ID);
	DOREPLIFETIME(UBankAccount, Balance);
}
