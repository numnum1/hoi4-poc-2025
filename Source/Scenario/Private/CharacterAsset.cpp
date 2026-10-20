// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.


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
