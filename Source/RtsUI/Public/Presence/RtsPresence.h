#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsPresence.generated.h"

class UBuildings;
class UCharacters;
class UDepartments;
class UOrganizations;

/**
 * Answers "who is where" — which organizations have a seat in a province, which province an
 * organization's root department sits in, how much room it has and how many members it has.
 * A UBlueprintFunctionLibrary like URtsResources/URtsActions: small static functions, facades
 * passed in rather than resolved here, so every function stays a pure function of its inputs.
 *
 * Exists apart from URtsActions because these four answer "who is where", not "what is allowed" —
 * and they are read from three places (the province screen, the join gate, the recruit gate), not
 * gated behind any one action.
 *
 * CollectOrganizationsIn walks every department linearly (Department -> Seat -> Province) — the
 * "Sammel-Getter ohne Rückwärtsindex" CLAUDE.md sanctions for the UI layer, never for a Mass
 * processor.
 */
UCLASS()
class RTSUI_API URtsPresence : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Every organization with at least one department seated in a building in InProvinceId. */
	UFUNCTION(BlueprintPure, Category="Rts Presence")
	static TArray<FOrganizationId> CollectOrganizationsIn(FProvinceId InProvinceId, UDepartments* InDepartments, UBuildings* InBuildings);

	/** The province InOrganizationId's root department is seated in, or an invalid id if it has
	 * none (a scenario-authored organization, or one whose root department was dissolved). */
	UFUNCTION(BlueprintPure, Category="Rts Presence")
	static FProvinceId FindSeatProvince(FOrganizationId InOrganizationId, UOrganizations* InOrganizations, UDepartments* InDepartments, UBuildings* InBuildings);

	/** The sum of the space every department InOrganizationId has claims — the member cap. */
	UFUNCTION(BlueprintPure, Category="Rts Presence")
	static int32 GetCapacity(FOrganizationId InOrganizationId, UDepartments* InDepartments);

	/** How many characters currently hold a position at InOrganizationId. */
	UFUNCTION(BlueprintPure, Category="Rts Presence")
	static int32 CountMembers(FOrganizationId InOrganizationId, UCharacters* InCharacters);

	/** Every character currently holding a position at InOrganizationId. Same scan as CountMembers,
	 * for the caller that needs the ids rather than just how many. */
	UFUNCTION(BlueprintPure, Category="Rts Presence")
	static TArray<FCharacterId> CollectMembers(FOrganizationId InOrganizationId, UCharacters* InCharacters);
};
