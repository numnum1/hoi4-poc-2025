// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAction.h"

UCharacterAction::UCharacterAction()
{
	ActionName = FText::FromString("Unnamed Action");
	Icon = FSlateNoResource();
}

void UCharacterAction::Click_Implementation(APlayerController* InPlayer, AHUD* InHUD, ARtsCharacter* InCharacter, UObject* InRoleContext)
{
	ensureMsgf(false, TEXT("Missing implementation for handle click in %s"), *GetClass()->GetName());
}