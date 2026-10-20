// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.


#include "NationAsset.h"

UNationAsset::UNationAsset()
{
	NationName = FText::FromString("Unnamed Nation");

	// Keep the FSlateBrush default (DrawAs=Image): FSlateNoResource bakes DrawAs=NoDrawType into
	// the CDO, and assigning a texture in the details panel only touches ResourceObject — the
	// flag would stay invisible no matter what the designer sets.
}
