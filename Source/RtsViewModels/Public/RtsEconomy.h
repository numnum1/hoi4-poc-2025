#pragma once

#include "CoreMinimal.h"
#include "RtsEconomyContext.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsEconomy.generated.h"

/**
 * The daily settlement: income, upkeep and wages for every alive organization, run once per
 * elapsed game day — see ARtsGameState for what ticks it. A UBlueprintFunctionLibrary rather than
 * a plain namespace, the same shape URtsActions/URtsResources use; SettleDay is a domain-crossing
 * mutation (reads Departments+Characters, writes Banking+Characters), so it lives in RtsViewModels
 * rather than any one domain module, with every facade it needs passed in through the context
 * instead of resolved here — the context stays a pure function of its inputs.
 */
UCLASS()
class RTSVIEWMODELS_API URtsEconomy : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Runs one day's income/upkeep/wages for every alive organization. See RtsEconomy.cpp for the
	 * formula and what happens when an organization's account cannot cover its costs. */
	UFUNCTION(BlueprintCallable, Category="Rts Economy")
	static void SettleDay(const FRtsEconomyContext& InContext);
};
