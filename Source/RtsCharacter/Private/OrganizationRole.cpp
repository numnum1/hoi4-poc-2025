// Fill out your copyright notice in the Description page of Project Settings.


#include "OrganizationRole.h"

#include "Net/UnrealNetwork.h"

UOrganizationRole::UOrganizationRole()
{
	Organization = nullptr;
	Position = nullptr;
}

void UOrganizationRole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UOrganizationRole, Organization);
	DOREPLIFETIME(UOrganizationRole, Position);
}

void UOrganizationRole::Setup(AOrganization* InOrganization, UOrganizationPosition* InPosition)
{
	verify(InOrganization);
	verify(InPosition);
	ensure(!Organization);
	ensure(!Position);
	
	Organization = InOrganization;
	Position = InPosition;
}
