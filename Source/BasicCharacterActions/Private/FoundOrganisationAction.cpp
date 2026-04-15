// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundOrganisationAction.h"

#include "RtsPlayerController.h"

UFoundOrganisationAction::UFoundOrganisationAction()
{
	ActionName = FText::FromString("Found Organisation");
}

void UFoundOrganisationAction::Click_Implementation(ARtsPlayerController* InPlayer, AHUD* InHUD, ARtsCharacter* InCharacter, UObject* InRoleContext)
{
	verify(InPlayer);	
	verify(InHUD);
	verify(InCharacter);
	
	InPlayer->FoundOrganisation(InCharacter);
}
