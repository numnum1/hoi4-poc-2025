#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "RtsActionContext.generated.h"

class UBanking;
class UBuildings;
class UCharacters;
class UDepartments;
class UOrganizationFocuses;
class UOrganizations;
class UProvinces;
class URoles;
class UTime;

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
 * Never assembled by hand: URtsHUDViewModel builds it from the character's own
 * positions, so OrganizationId and RoleId always come from the same position and cannot be
 * mismatched. The shared gate in RtsActions.cpp verifies that anyway before anything runs —
 * a context that reached C++ from Blueprint is a claim, not a fact.
 *
 * OrganizationId/RoleId are only meaningful when Scope is RAS_Position. Characters/Organizations/
 * Roles are the facades the gates and the execute functions read through — the context carries them
 * instead of URtsActions resolving a subsystem itself, so every function in URtsActions stays a
 * pure function of its inputs (see NationActions.h for the same shape, domain-local instead of
 * cross-domain).
 */
USTRUCT(BlueprintType)
struct RTSUI_API FRtsActionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Action")
	ERtsActionScope Scope = ERtsActionScope::RAS_Self;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	FCharacterId ActorId;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	FOrganizationId OrganizationId;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	FRoleId RoleId;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UCharacters> Characters;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UOrganizations> Organizations;

	/** Founding creates the organization's root department, and seating one is the only place the
	 * building-space rule can be checked — it needs UBuildingType from Buildings and the space
	 * already claimed from here, which no single domain module may know at once. */
	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UDepartments> Departments;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<URoles> Roles;

	/** Founding puts up a building and pays for it, so both facades ride along for the same reason
	 * the three above do: URtsActions stays a pure function of its inputs. */
	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UBuildings> Buildings;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UBanking> Banking;

	/** Founding checks the site province still exists before putting a building up there — also
	 * where the WeltMap's green/red founding-site preview reads from, see
	 * URtsActions::CanFoundHeadquartersIn. */
	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UProvinces> Provinces;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UOrganizationFocuses> OrganizationFocuses;

	UPROPERTY(BlueprintReadOnly, Category="Action")
	TObjectPtr<UTime> Time;
};
