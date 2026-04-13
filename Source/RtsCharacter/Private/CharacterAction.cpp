// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAction.h"

UCharacterAction::UCharacterAction()
{
}

void UCharacterAction::Click_Implementation(ARtsCharacter* InWithCharacterContext, APlayerController* InByPlayerController)
{
	ensureMsgf(false, TEXT("%s is missing a custom 'OnClicked' implementation"), *GetClass()->GetName());
}
