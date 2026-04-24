// Fill out your copyright notice in the Description page of Project Settings.


#include "BankAccountHolder.h"

#include "Net/UnrealNetwork.h"

UBankAccountHolder::UBankAccountHolder()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);
}

void UBankAccountHolder::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UBankAccountHolder, HoldAccounts);
}
