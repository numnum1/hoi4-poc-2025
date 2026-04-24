// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Bank.generated.h"

class UBankAccount;

UCLASS()
class BANKINGSYSTEM_API ABank : public AInfo
{
	GENERATED_BODY()

public:
	ABank();
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<UBankAccount*> Accounts;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
};
