// Fill out your copyright notice in the Description page of Project Settings.


#include "Species.h"

USpecies::USpecies()
{
	Category = nullptr;
	SpeciesName = FText::FromString("Unnamed Species");
	Traits = {};
}
