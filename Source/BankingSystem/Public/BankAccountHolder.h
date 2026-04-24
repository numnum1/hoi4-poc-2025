// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BankAccountReference.h"
#include "Components/ActorComponent.h"
#include "BankAccountHolder.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBankAccountHolderEvent);

UCLASS(ClassGroup=(RTS), meta=(BlueprintSpawnableComponent))
class BANKINGSYSTEM_API UBankAccountHolder : public UActorComponent
{
	GENERATED_BODY()

public:
	UBankAccountHolder();

	UPROPERTY(BlueprintAssignable)
	FBankAccountHolderEvent OnAccountsChanged;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated, ReplicatedUsing=OnRep_Accounts)
	TArray<FBankAccountReference> HoldAccounts;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UFUNCTION()
	FORCEINLINE void OnRep_Accounts()
	{
		OnAccountsChanged.Broadcast();
	}
};
