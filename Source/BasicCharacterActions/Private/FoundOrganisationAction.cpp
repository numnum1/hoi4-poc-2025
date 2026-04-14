// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundOrganisationAction.h"

UFoundOrganisationAction::UFoundOrganisationAction()
{
	ActionName = FText::FromString("Found Organisation");
}

void UFoundOrganisationAction::Click_Implementation(APlayerController* InPlayer, AHUD* InHUD, ARtsCharacter* InCharacter, UObject* InRoleContext)
{
	verify(InPlayer);	
	verify(InHUD);
	verify(InCharacter);
	
	InCharacter->
}
