// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAction.h"

#include "Net/UnrealNetwork.h"

UCharacterAction::UCharacterAction()
{
	ActionName = FText::FromString("Unnamed Action");
	Icon = FSlateNoResource();
}

void UCharacterAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCharacterAction, ActionName);
	DOREPLIFETIME(UCharacterAction, Icon);
}

void UCharacterAction::Click_Implementation()
{
	ensureMsgf(false, TEXT("Override this"));
}
