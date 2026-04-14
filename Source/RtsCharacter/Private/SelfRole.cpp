// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfRole.h"

#include "CharacterAction.h"
#include "RtsCharacter.h"


USelfRole::USelfRole()
{
}

void USelfRole::RegisterAddedToCharacter_Implementation(ARtsCharacter* InCharacter)
{
	verify(InCharacter);
	
	UpdateNameAndIcon(InCharacter);
	
	InCharacter->OnCharacterInfoChanged.AddDynamic(this, &USelfRole::UpdateNameAndIcon);
}

void USelfRole::RegisterRemoveFromCharacter_Implementation(ARtsCharacter* InCharacter)
{
	verify(InCharacter);
	
	InCharacter->OnCharacterInfoChanged.RemoveDynamic(this, &USelfRole::UpdateNameAndIcon);
}

void USelfRole::UpdateNameAndIcon(ARtsCharacter* InCharacter)
{
	verify(InCharacter);
	
	SetRoleName(InCharacter->GetCharacterInfo().Name);
	SetIcon(InCharacter->GetCharacterInfo().Portrait);
}
