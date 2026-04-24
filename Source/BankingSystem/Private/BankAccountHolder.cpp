// Fill out your copyright notice in the Description page of Project Settings.


#include "BankAccountHolder.h"

#include "Bank.h"
#include "Net/UnrealNetwork.h"

UBankAccountHolder::UBankAccountHolder()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);
	
	HoldAccounts = {};
}

void UBankAccountHolder::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UBankAccountHolder, HoldAccounts);
}

int32 UBankAccountHolder::Count(URoleResource*& OutCurrency) const
{
	int32 Res = 0;
	OutCurrency = nullptr;
	for (const auto& Account : HoldAccounts)
	{
		verify(Account.Bank);
		OutCurrency = Account.Bank->GetCurrency();
		Res += Account.Request();
	}
	
	return Res;
}