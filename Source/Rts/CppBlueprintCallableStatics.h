// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CppBlueprintCallableStatics.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API UCppBlueprintCallableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool InRange(const FInt32Range& InRange, int32 InValue);
};
