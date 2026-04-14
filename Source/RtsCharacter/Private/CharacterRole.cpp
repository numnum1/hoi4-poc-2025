// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterRole.h"

UCharacterRole::UCharacterRole()
{
}

UObject* UCharacterRole::GetRoleContext_Implementation() const
{
	return nullptr;
}

void UCharacterRole::PlayCharacter_Implementation(ARtsCharacter* InCharacter)
{
}

void UCharacterRole::RegisterAddedToCharacter_Implementation(ARtsCharacter* InCharacter)
{
}

void UCharacterRole::RegisterRemoveFromCharacter_Implementation(ARtsCharacter* InCharacter)
{
}

void UCharacterRole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCharacterRole, RoleName);
	DOREPLIFETIME(UCharacterRole, Icon);
}
