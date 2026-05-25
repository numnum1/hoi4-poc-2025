// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAsset.h"

UCharacterAsset::UCharacterAsset()
{
	SimTier = ECharacterSimTier::CST_None;
	CharacterName = FText::FromString("Unnamed Character");
	Portrait = FSlateNoResource();
	ProvinceOfBirth = nullptr;
	BaseRace = nullptr;
	CurrentProvince = nullptr;
}
