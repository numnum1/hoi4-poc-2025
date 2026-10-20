// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.


#include "ProvinceAsset.h"

UProvinceAsset::UProvinceAsset()
{
	MapId = NAME_None;
	
	ProvinceName = FText::FromString("Unnamed Province");
	Picture = FSlateNoResource();
}
