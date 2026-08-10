#pragma once

#include "CoreMinimal.h"
#include "BuildingFunction.h"
#include "RtsIds.h"
#include "Mass/EntityElementTypes.h"
#include "DepartmentFragments.generated.h"

/**
 * Back-reference from an entity to its stable FDepartmentId. Same role as
 * FOrganizationIndexFragment: the id is what every other domain and every view model addresses a
 * department by, so each entity carries the one it was created for.
 */
USTRUCT()
struct ORGANIZATIONS_API FDepartmentIndexFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FDepartmentId Id;
};

/**
 * Which organization this department belongs to. Carried on every department rather than only on
 * the root: a department never changes organization, so this cannot drift, and "which organization
 * is this character in" would otherwise walk the parent chain on every single ask.
 */
USTRUCT()
struct ORGANIZATIONS_API FDepartmentOrganizationFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FOrganizationId OrganizationId;
};

/**
 * The department this one reports to. An invalid id means this is the root — the one an
 * organization points at.
 *
 * Only the parent link is stored, never a children array: the children of a department are found
 * by scanning for this field, so there is one source of truth and nothing to keep in sync. That
 * scan is linear and belongs to the UI layer, never inside a Mass processor.
 *
 * The tree is how an organization spreads out. A church wanting a presence in three provinces
 * gets three sub-departments, one per church building — a branch office *is* a sub-department, not
 * a second address of the same one. That is why a department has exactly one seat below.
 */
USTRUCT()
struct ORGANIZATIONS_API FDepartmentParentFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FDepartmentId ParentId;
};

/**
 * Where the department sits, and how much of that building's space for its function it takes up.
 * A building hosts several departments, so the seat is shared: the sum of Space across the
 * departments seated in one building for one function may not exceed what the building type offers
 * (UBuildingType::GetSpaceFor). Checking that needs both domains and therefore lives in
 * URtsActions, not here.
 *
 * Seating is not ownership. The building belongs to a character
 * (FBuildingCharacterOwnerFragment) — possibly a different one than the organization's founder.
 */
USTRUCT()
struct ORGANIZATIONS_API FDepartmentSeatFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FBuildingId BuildingId;

	UPROPERTY()
	int32 Space = 0;
};

/**
 * What the department does. This is the whole of "what kind of thing is this" — there is no
 * department type asset and no organization type. An organization is a church because its
 * departments pray. See EBuildingFunction.
 */
USTRUCT()
struct ORGANIZATIONS_API FDepartmentFunctionFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	EBuildingFunction Function = EBuildingFunction::BF_Bureaucracy;
};

USTRUCT()
struct ORGANIZATIONS_API FDepartmentTag : public FMassTag
{
	GENERATED_BODY()
};
