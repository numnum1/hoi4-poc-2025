#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NationActions.generated.h"

class UNations;
class UOrganizations;

/**
 * Hardcoded gameplay actions that target a nation, as small static functions rather than a method
 * on a per-instance proxy — same "enum+switch, not polymorphism" spirit as URtsActions, just
 * domain-local instead of cross-domain. A caller already holds the facades it needs (from
 * ARtsGameState); this class never resolves them itself, so every function stays a pure function
 * of its inputs.
 */
UCLASS()
class NATIONS_API UNationActions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Settles InOrganizationId in InNationId. No-op (returns false) if InNationId is not alive or
	 * the organization is already settled there — SettleNation itself is idempotent. */
	UFUNCTION(BlueprintCallable, Category="Nation Actions")
	static bool RegisterOrganization(UNations* InNations, UOrganizations* InOrganizations, FNationId InNationId, FOrganizationId InOrganizationId);
};
