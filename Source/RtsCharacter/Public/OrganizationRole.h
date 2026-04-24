// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JuristicPerson.h"
#include "CoreMinimal.h"
#include "CharacterRole.h"
#include "OrganizationRole.generated.h"

class UOrganizationPosition;
/**
 * 
 */
UCLASS()
class RTSCHARACTER_API UOrganizationRole : public UCharacterRole, public IJuristicPerson
{
	GENERATED_BODY()
public:
	UOrganizationRole();
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	class AOrganization* Organization;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	UOrganizationPosition* Position;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Setup(AOrganization* InOrganization, UOrganizationPosition* InPosition);

	// Interfaces
	virtual void BuyProperty_Implementation() override;
private:
	UFUNCTION()
	void UnbindFromOrganization(AActor* InRef);
	UFUNCTION()
	void UpdateInfoFromOrganization();
};
