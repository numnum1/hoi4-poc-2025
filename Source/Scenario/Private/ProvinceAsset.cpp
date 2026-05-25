// Fill out your copyright notice in the Description page of Project Settings.


#include "ProvinceAsset.h"

UProvinceAsset::UProvinceAsset()
{
	MapId = NAME_None;
	
	ProvinceName = FText::FromString("Unnamed Province");
	Picture = FSlateNoResource();
}
