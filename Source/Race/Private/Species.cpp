// Fill out your copyright notice in the Description page of Project Settings.


#include "Species.h"
#include "LifeType.h"

USpecies::USpecies()
{
	Category = nullptr;
	Type = nullptr;
	SpeciesName = FText::FromString("Unnamed Species");
	Traits = {};
}

TArray<URaceTrait*> USpecies::CollectTraits_Implementation() const
{
	TSet<URaceTrait*> Res = TSet(Traits);
	verify(Type);
	for (URaceTrait* Trait : Type->Traits)
	{
		ensure(Trait);
		Res.Add(Trait);
	}
	return Res.Array();
}
