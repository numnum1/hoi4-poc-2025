// Fill out your copyright notice in the Description page of Project Settings.


#include "BuyPropertyAction.h"

#include "JuristicPerson.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UBuyPropertyAction::UBuyPropertyAction()
{
	ActionName = FText::FromString("Buy Property Action");
	
	static ConstructorHelpers::FObjectFinder<UTexture2D> DataObj(TEXT("/Script/Engine.Texture2D'/Game/ProIconPack/Textures/Generic_Icons/64x64/T_Home_64x64.T_Home_64x64'"));
	Icon = UWidgetBlueprintLibrary::MakeBrushFromTexture(DataObj.Object);
}

void UBuyPropertyAction::Click_Implementation(UObject* InRoleContext)
{
	verify(InRoleContext);
	verify(InRoleContext->Implements<UJuristicPerson>());
	
	IJuristicPerson::Execute_BuyProperty(InRoleContext);
}
