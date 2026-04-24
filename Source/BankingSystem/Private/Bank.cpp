// Fill out your copyright notice in the Description page of Project Settings.


#include "Bank.h"

#include "BankAccount.h"
#include "Net/UnrealNetwork.h"

ABank::ABank()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicateUsingRegisteredSubObjectList = true;
	
	bReplicates = true;
	bAlwaysRelevant = true;
	
	Currency = nullptr;
	Accounts = {};
}

void ABank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABank, Accounts);
}

int32 ABank::GetBalance(const FGuid& InAccountID) const
{
	for (UBankAccount* Account : Accounts)
	{
		if (Account->GetID() == InAccountID)
		{
			return Account->GetBalance();
		}
	}
	return 0;
}

void ABank::BeginPlay()
{
	Super::BeginPlay();
	
	for (UBankAccount* Account : Accounts)
	{
		AddReplicatedSubObject(Account);
	}
}
