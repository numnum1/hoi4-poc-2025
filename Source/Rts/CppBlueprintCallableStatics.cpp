// Fill out your copyright notice in the Description page of Project Settings.


#include "CppBlueprintCallableStatics.h"

bool UCppBlueprintCallableStatics::InRange(const FInt32Range& InRange, const int32 InValue)
{
	return InRange.Contains(InValue);
}
