// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.


#include "CppBlueprintCallableStatics.h"

bool UCppBlueprintCallableStatics::InRange(const FInt32Range& InRange, const int32 InValue)
{
	return InRange.Contains(InValue);
}
