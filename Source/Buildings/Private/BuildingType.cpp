// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingType.h"

UBuildingType::UBuildingType()
{
	BuildingName = FText::FromString("Unnamed Building");
	Icon = FSlateNoResource();
	BuildingSlotName = NAME_None;
	MaxPerProvince = 0;
}
