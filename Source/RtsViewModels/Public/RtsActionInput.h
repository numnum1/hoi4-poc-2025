#pragma once

#include "CoreMinimal.h"
#include "BuildingKind.h"
#include "RtsIds.h"
#include "RtsActionInput.generated.h"

/**
 * What an action needs from the player on top of FRtsActionContext — the values a modal collects
 * before the action runs. One flat struct covering the handful of actions that ask for input,
 * rather than a type per action; the same reason EAction is a single enum.
 *
 * Like the context, this is a claim rather than a fact: URtsActions::GetInvalidInputReason
 * re-validates it, and UPlayerCharacterViewModel::ConfirmModal runs that check again on confirm
 * instead of trusting whatever was last pushed into the widget.
 */
USTRUCT(BlueprintType)
struct RTSVIEWMODELS_API FRtsActionInput
{
	GENERATED_BODY()

	/** EA_FoundOrganization: the name the player typed for the new organization. */
	UPROPERTY(BlueprintReadWrite, Category="Action")
	FText OrganizationName;

	/** EA_FoundOrganization: which headquarters to put up. This is the whole of what the new
	 * organization *is* — there is no separate organization type to pick. */
	UPROPERTY(BlueprintReadWrite, Category="Action")
	EBuildingKind HeadquartersKind = EBuildingKind::BK_None;

	/** EA_FoundOrganization: where that headquarters goes, set by clicking the world map. */
	UPROPERTY(BlueprintReadWrite, Category="Action")
	FProvinceId HeadquartersProvinceId;
};
