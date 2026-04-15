// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundOrganisationAction.h"

#include "JuristicPerson/Public/JuristicPerson.h"

UFoundOrganisationAction::UFoundOrganisationAction()
{
	ActionName = FText::FromString("Found Organisation");
}

void UFoundOrganisationAction::Click_Implementation(AActor* InRoleContext)
{
	verify(InRoleContext);
	verify(InRoleContext->Implements<UJuristicPerson>());
	
	IJuristicPerson::Execute_FoundOrganisation(InRoleContext);
}
