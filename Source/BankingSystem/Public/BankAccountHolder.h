// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BankAccountReference.h"
#include "BankingTransaction.h"
#include "Components/ActorComponent.h"
#include "BankAccountHolder.generated.h"

class URoleResource;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBankAccountHolderEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTransactionHappenedEvent, const FBankAccountReference&, InAccountReference, const FBankingTransaction&, InTransaction);

UCLASS(ClassGroup=(RTS), meta=(BlueprintSpawnableComponent))
class BANKINGSYSTEM_API UBankAccountHolder : public UActorComponent
{
	GENERATED_BODY()

public:
	UBankAccountHolder();

	UPROPERTY(BlueprintAssignable)
	FBankAccountHolderEvent OnAccountsChanged;

	UPROPERTY(BlueprintAssignable)
	FOnTransactionHappenedEvent OnTransactionHappened;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Accounts)
	TArray<FBankAccountReference> HoldAccounts;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	int32 Count(URoleResource*& OutCurrency) const;
private:
	UFUNCTION()
	FORCEINLINE void OnRep_Accounts()
	{
		OnAccountsChanged.Broadcast();
	}
};
