// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateOrganizationAction.h"
#include "RtsCharacter.h"
#include "Net/UnrealNetwork.h"

UCreateOrganizationAction::UCreateOrganizationAction()
{
	Character = nullptr;
}

void UCreateOrganizationAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCreateOrganizationAction, Character);
}

void UCreateOrganizationAction::SetCharacter(ARtsCharacter* NewCharacter)
{
	Character = NewCharacter;
}

void UCreateOrganizationAction::Click_Implementation()
{
	verify(Character);
	
	Character->FoundOrganisation.Broadcast();
}
