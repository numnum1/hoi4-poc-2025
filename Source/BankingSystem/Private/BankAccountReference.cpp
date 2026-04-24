// Fill out your copyright notice in the Description page of Project Settings.


#include "BankAccountReference.h"

#include "Bank.h"

int32 FBankAccountReference::Request() const
{
	verify(Bank);
	verify(AccountID.IsValid());
	
	return Bank->GetBalance(AccountID);
}
