#pragma once

#include "CoreMinimal.h"
#include "Resources/ResourceType.h"
#include "Resources/RtsResourceContext.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RtsResources.generated.h"

/**
 * The resource catalogue, hardcoded as a set of small switches over EResourceType rather than a
 * lookup table or a UObject subclass per resource. GetAmount is the one seam every resource's
 * backend eventually plugs into — today it answers from what already exists (Manpower, derived
 * from character positions) or returns 0 with a comment naming what will answer it later (Money,
 * Crystals). Everything else in this class is display data for the generic resource row.
 *
 * A UBlueprintFunctionLibrary rather than a plain namespace, the same shape URtsActions uses.
 */
UCLASS()
class RTSVIEWMODELS_API URtsResources : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Which resources InOwner's panel shows, in display order. A resource on this list is shown
	 * whatever its amount turns out to be — "has none of it right now" is a number, not a reason to
	 * hide the row, so nothing here ever consults GetAmount.
	 *
	 * Splits on nothing but the owner kind: the two cases below answer it.
	 */
	static TArray<EResourceType> FindResourcesToDisplay(const FResourceOwner& InOwner);

	/** The character case of FindResourcesToDisplay. */
	static TArray<EResourceType> FindResourcesToDisplayForCharacter(FCharacterId InCharacterId);

	/** The organization case of FindResourcesToDisplay. */
	static TArray<EResourceType> FindResourcesToDisplayForOrganization(FOrganizationId InOrganizationId);

	static FText GetDisplayName(EResourceType InResource);
	static FSlateBrush GetIcon(EResourceType InResource);

	/** The sub-values InResource offers, in display order. RD_Total is always first. */
	static TArray<EResourceDetail> GetDetails(EResourceType InResource);

	/** Takes both, because "Total" reads differently per resource (balance vs headcount). */
	static FText GetDetailName(EResourceType InResource, EResourceDetail InDetail);

	/**
	 * THE seam: one case per (resource, detail), and everything else in this system is display
	 * around this. A pure function of (resource, detail, context) — the context carries the
	 * facades this needs, so it never resolves a subsystem itself.
	 */
	static int32 GetAmount(EResourceType InResource, EResourceDetail InDetail, const FRtsResourceContext& InContext);

	/** Money reads as a balance, everything else as a plain count. */
	static FText FormatAmount(EResourceType InResource, int32 InAmount);
};
