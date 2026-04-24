// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Bank.generated.h"

class URoleResource;
class UBankAccount;

UCLASS()
class BANKINGSYSTEM_API ABank : public AInfo
{
	GENERATED_BODY()

public:
	ABank();
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	URoleResource* Currency;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	TArray<UBankAccount*> Accounts;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FORCEINLINE URoleResource* GetCurrency() const { return Currency; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetBalance(const FGuid& InAccountID) const;
protected:
	virtual void BeginPlay() override;
};
