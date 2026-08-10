// Fill out your copyright notice in the Description page of Project Settings.


#include "NationAsset.h"

UNationAsset::UNationAsset()
{
	NationName = FText::FromString("Unnamed Nation");

	// Keep the FSlateBrush default (DrawAs=Image): FSlateNoResource bakes DrawAs=NoDrawType into
	// the CDO, and assigning a texture in the details panel only touches ResourceObject — the
	// flag would stay invisible no matter what the designer sets.
}
