#pragma once

#include "CoreMinimal.h"
#include "Actions/RtsActionContext.h"
#include "Actions/RtsActionInput.h"
#include "Characters.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsActions.generated.h"

class UBanking;
class UProvinces;

/**
 * The action catalogue, hardcoded as one set of named functions per action rather than a lookup
 * table, a UObject subclass per action, or a switch over an action enum. There is no runtime action
 * identity at all: every caller already knows which action it means at the call site, so it calls
 * that action's functions by name. Adding an action means adding its three functions here —
 * nothing else, and nothing existing has to grow a case.
 *
 * Each action has the same three:
 *   Get<Action>BlockedReason(Context)          — may this run at all, empty means yes
 *   Get<Action>InvalidInputReason(Ctx, Input)  — are the collected values usable, empty means yes
 *   Execute<Action>(Context, Input)            — run it
 *
 * The two gates are separate because they answer different questions at different times: whether
 * the action may be offered, versus whether what the player typed is acceptable. A widget re-runs
 * the second on every keystroke; both run again on confirm. Actions that collect nothing have no
 * input gate.
 *
 * A context is a claim, not a fact: a blocked-reason function re-derives everything from the
 * character itself, and position-scoped actions verify through the shared gate that the character
 * really holds the (organization, role) pair the context claims. All of it stays a pure function of
 * its inputs — the context carries the facades, so nothing here resolves a subsystem itself.
 *
 * A UBlueprintFunctionLibrary rather than a plain namespace, the same shape UScheduleStatics uses,
 * so a widget can call the pure queries directly if it ever needs to.
 */
UCLASS()
class RTSUI_API URtsActions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FText GetFoundOrganizationBlockedReason(const FRtsActionContext& InContext);
	static FText GetFoundOrganizationInvalidInputReason(const FRtsActionContext& InContext, const FRtsActionInput& InInput);
	static void ExecuteFoundOrganization(const FRtsActionContext& InContext, const FRtsActionInput& InInput);

	static FText GetJoinOrganizationBlockedReason(const FRtsActionContext& InContext);
	static FText GetJoinOrganizationInvalidInputReason(const FRtsActionContext& InContext, const FRtsActionInput& InInput);
	static void ExecuteJoinOrganization(const FRtsActionContext& InContext, const FRtsActionInput& InInput);

	static FText GetLeaveOrganizationBlockedReason(const FRtsActionContext& InContext);
	static void ExecuteLeaveOrganization(const FRtsActionContext& InContext);

	static FText GetRecruitCharacterBlockedReason(const FRtsActionContext& InContext);
	static FText GetRecruitCharacterInvalidInputReason(const FRtsActionContext& InContext, const FRtsActionInput& InInput);
	static void ExecuteRecruitCharacter(const FRtsActionContext& InContext, const FRtsActionInput& InInput);

	/**
	 * Whether a headquarters could be founded in InProvinceId right now — the province-specific
	 * slice of GetFoundOrganizationInvalidInputReason, pulled out so the WeltMap can color
	 * provinces by founding eligibility while the player picks a site without duplicating the
	 * rule. Checks that the province still exists and belongs to nobody or to InActingNationId —
	 * further province-specific founding restrictions (terrain, ...) get added here once they
	 * exist, not into the map's Blueprint.
	 */
	UFUNCTION(BlueprintPure, Category="Rts Actions")
	static bool CanFoundHeadquartersIn(FProvinceId InProvinceId, UProvinces* InProvinces, FNationId InActingNationId);

	/**
	 * Everything the character has, across every account they hold — a character's cash is one pot,
	 * the same sum the money row in the resource panel shows (see URtsResources, RD_Total). Public
	 * because every picker that prices something against the buyer needs it (the build menu's
	 * affordability, the founding gate below), and three copies of the loop would be three places
	 * for "one pot" to stop being true.
	 */
	UFUNCTION(BlueprintPure, Category="Rts Actions")
	static int32 GetCharacterFunds(const UBanking* InBanking, FCharacterId InCharacterId);
};
