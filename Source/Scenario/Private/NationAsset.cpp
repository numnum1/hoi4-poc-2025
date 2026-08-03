// Fill out your copyright notice in the Description page of Project Settings.


#include "NationAsset.h"

UNationAsset::UNationAsset()
{
	NationName = FText::FromString("Unnamed Nation");

	// Flag deliberately keeps the FSlateBrush default (DrawAs=Image): FSlateNoResource would bake
	// DrawAs=NoDrawType into the CDO, and assigning a texture in the details panel only touches
	// ResourceObject — the flag would stay invisible no matter what the designer sets.
}
