#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "RtsActionContext.generated.h"

class UCharacterProxy;

/** Which role the acting character plays for this action: themselves, or one of their positions. */
UENUM(BlueprintType)
enum class ERtsActionScope : uint8
{
	RAS_Self     UMETA(DisplayName = "Self"),
	RAS_Position UMETA(DisplayName = "Position"),
};

/**
 * Everything an action needs to check availability and run, for one character in one scope.
 *
 * Never assembled by hand: UCharacterActionsViewModel builds it from the character's own
 * positions, so OrganizationId and RoleId always come from the same position and cannot be
 * mismatched. The shared gate in RtsActions.cpp verifies that anyway before anything runs —
 * a context that reached C++ from Blueprint is a claim, not a fact.
 *
 * OrganizationId/RoleId are only meaningful when Scope is RAS_Position.
 */
USTRUCT(BlueprintType)
struct RTSPROXY_API FRtsActionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Action")
	ERtsActionScope Scope = ERtsActionScope::RAS_Self;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UCharacterProxy> Actor;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	FOrganizationId OrganizationId;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	FRoleId RoleId;
};
