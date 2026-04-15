// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfRole.h"
#include "RtsCharacter.h"


USelfRole::USelfRole()
{
}

void USelfRole::SetupWithContext_Implementation(AActor* InContext)
{
	ARtsCharacter* RtsCharacter = Cast<ARtsCharacter>(InContext);
	
	verify(RtsCharacter);

	Super::SetupWithContext_Implementation(InContext);
	
	UpdateNameAndIcon(RtsCharacter);
	RegisterAddedToCharacter(RtsCharacter);
	RtsCharacter->OnDestroyed.AddDynamic(this, &USelfRole::RegisterRemoveFromCharacter);
}

void USelfRole::RegisterAddedToCharacter(ARtsCharacter* InCharacter)
{
	verify(InCharacter);
	
	UpdateNameAndIcon(InCharacter);
	
	InCharacter->OnCharacterInfoChanged.AddDynamic(this, &USelfRole::UpdateNameAndIcon);
}

void USelfRole::RegisterRemoveFromCharacter(AActor* InCharacter)
{
	ARtsCharacter* RtsCharacter = Cast<ARtsCharacter>(InCharacter);
	
	verify(RtsCharacter);
	
	RtsCharacter->OnCharacterInfoChanged.RemoveDynamic(this, &USelfRole::UpdateNameAndIcon);
}

void USelfRole::UpdateNameAndIcon(ARtsCharacter* InCharacter)
{
	verify(InCharacter);
	
	SetRoleName(InCharacter->GetCharacterInfo().Name);
	SetIcon(InCharacter->GetCharacterInfo().Portrait);
}
