// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterRole.h"
#include "JuristicPerson.h"
#include "SelfRole.generated.h"

class ARtsCharacter;

UCLASS(BlueprintType)
class RTSCHARACTER_API USelfRole : public UCharacterRole, public IJuristicPerson
{
	GENERATED_BODY()

public:
	USelfRole();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), BlueprintSetter=SetCharacter, Replicated)
	ARtsCharacter* Character;
	
public:
	
	UFUNCTION(BlueprintSetter)
	void SetCharacter(ARtsCharacter* NewCharacter);
	
private:
	
	void RegisterAddedToCharacter(ARtsCharacter* InCharacter);
	UFUNCTION()
	void HandleCharacterDestroyed(AActor* InCharacter);
	
	UFUNCTION()
	void UpdateNameAndIcon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Interfaces
	virtual void FoundOrganisation_Implementation() override;
	
	
};
