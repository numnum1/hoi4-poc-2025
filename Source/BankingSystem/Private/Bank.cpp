// Fill out your copyright notice in the Description page of Project Settings.


#include "Bank.h"

#include "BankAccount.h"
#include "Net/UnrealNetwork.h"

ABank::ABank()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bReplicates = true;
	bAlwaysRelevant = true;
	
	Accounts = {};
}

void ABank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABank, Accounts);
}

void ABank::BeginPlay()
{
	Super::BeginPlay();
	
	for (UBankAccount* Account : Accounts)
	{
		AddReplicatedSubObject(Account);
	}
}
