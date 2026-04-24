// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfRole.h"

#include "BankAccountHolder.h"
#include "RtsCharacter.h"
#include "Net/UnrealNetwork.h"


USelfRole::USelfRole()
{
}

void USelfRole::SetCharacter(ARtsCharacter* NewCharacter)
{
	verify(NewCharacter);
	verify(!Character);
	
	Character = NewCharacter;
	UpdateNameAndIcon();
	Character->OnDestroyed.AddDynamic(this, &USelfRole::HandleCharacterDestroyed);
	Character->OnCharacterInfoChanged.AddDynamic(this, &USelfRole::UpdateNameAndIcon);
	
	UBankAccountHolder* BankAccountHolder = Character->GetComponentByClass<UBankAccountHolder>();
	URoleResource* Currency = nullptr;
	int32 Count = BankAccountHolder->Count(Currency);
	PutResource(FRoleResourceCount{Currency, Count});
}

void USelfRole::HandleCharacterDestroyed(AActor* InCharacter)
{
	Character->OnDestroyed.RemoveDynamic(this, &USelfRole::HandleCharacterDestroyed);
	Character->OnCharacterInfoChanged.RemoveDynamic(this, &USelfRole::UpdateNameAndIcon);
	SetRoleName(FText::FromString("Missing character"));
	SetIcon(FSlateNoResource());
}

void USelfRole::UpdateNameAndIcon()
{
	SetRoleName(Character->GetCharacterInfo().Name);
	SetIcon(Character->GetCharacterInfo().Portrait);
}

void USelfRole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(USelfRole, Character);
}

void USelfRole::FoundOrganisation_Implementation()
{
	verify(Character);

	Character->FoundOrganisation.Broadcast();
}
