#pragma once

#include "CoreMinimal.h"
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
};
