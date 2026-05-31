// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceAsset.h"

TArray<URaceTraitAsset*> UGenomeAsset::CollectRaceTraits_Implementation() const
{
	return {};
}

URaceTraitAsset::URaceTraitAsset()
{
	TraitName = FText::FromString("Unnamed Trait");
	TraitDescription = FText::FromString("");
}

ULifeTypeAsset::ULifeTypeAsset()
{
	LifeTypeName = FText::FromString("Unnamed Life Type");
	Traits = {};
}

URaceCategoryAsset::URaceCategoryAsset()
{
	CategoryName = FText::FromString("Unnamed Category");
}

USpeciesAsset::USpeciesAsset()
{
	SpeciesName = FText::FromString("Unnamed Species");
	Traits = {};
	LifeType = nullptr;
	Category = nullptr;
}

TArray<URaceTraitAsset*> USpeciesAsset::CollectRaceTraits_Implementation() const
{
	TArray<URaceTraitAsset*> Res = Traits;
	if (LifeType)
	{
		for (URaceTraitAsset* T : LifeType->GetTraits())
		{
			Res.AddUnique(T);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Species '%s' is missing life type"), *SpeciesName.ToString());
	}
	return Res;
}

URaceAsset::URaceAsset()
{
	RaceName = FText::FromString("Unnamed Race");
	Species = nullptr;
	Traits = {};
}

TArray<URaceTraitAsset*> URaceAsset::CollectRaceTraits_Implementation() const
{
	TArray<URaceTraitAsset*> Res = Traits;
	if (Species)
	{
		for (URaceTraitAsset* T : Species->CollectRaceTraits())
		{
			Res.AddUnique(T);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Race '%s' is missing species"), *RaceName.ToString());
	}
	return Res;
}
