// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAction.h"
#include "CreateOrganizationAction.generated.h"

class ARtsCharacter;
/**
 * 
 */
UCLASS()
class RTSCHARACTER_API UCreateOrganizationAction : public UCharacterAction
{
	GENERATED_BODY()
public:
	UCreateOrganizationAction();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Replicated)
	ARtsCharacter* Character;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetCharacter(ARtsCharacter* NewCharacter);
	
	virtual void Click_Implementation() override;
};
