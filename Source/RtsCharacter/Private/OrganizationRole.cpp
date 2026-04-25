// Fill out your copyright notice in the Description page of Project Settings.


#include "OrganizationRole.h"

#include "BuyPropertyAction.h"
#include "Organisation.h"
#include "OrganizationPosition.h"
#include "Net/UnrealNetwork.h"

UOrganizationRole::UOrganizationRole()
{
	Organization = nullptr;
	Position = nullptr;
	
	AddAction(UBuyPropertyAction::StaticClass()->GetDefaultObject<UBuyPropertyAction>());
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
	
	Organization->OnDestroyed.AddDynamic(this, &UOrganizationRole::UnbindFromOrganization);
	Organization->OnInfoChanged.AddDynamic(this, &UOrganizationRole::UpdateInfoFromOrganization);
	
	UpdateInfoFromOrganization();
}

void UOrganizationRole::BuyProperty_Implementation()
{
	verify(Organization);
	
	Organization->BuyProperty();
}

void UOrganizationRole::UnbindFromOrganization(AActor* InRef)
{
	ensure(InRef == Organization);
	
	Organization->OnDestroyed.RemoveDynamic(this, &UOrganizationRole::UnbindFromOrganization);
	Organization->OnInfoChanged.RemoveDynamic(this, &UOrganizationRole::UpdateInfoFromOrganization);
	
	SetRoleName(FText::FromString("Missing organization"));
	SetIcon(FSlateNoResource());
	SetTitle(FText::FromString(""));
}

void UOrganizationRole::UpdateInfoFromOrganization()
{
	SetRoleName(Organization->GetInfo().Name);
	SetIcon(Organization->GetInfo().Logo);
	SetTitle(Position->Title);
}
