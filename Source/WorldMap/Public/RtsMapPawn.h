#pragma once

#include "CoreMinimal.h"
#include "BA_MapPawn.h"
#include "RtsIds.h"
#include "RtsMapPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRtsMapProvinceClicked, FProvinceId, ProvinceId);

/**
 * The map camera pawn, and where a click on the world becomes a province.
 *
 * Exists because ABA_MapPawn::Select is a BlueprintImplementableEvent, so the plugin's click can
 * only be picked up in an event graph — and a graph is exactly where this codebase does not put
 * logic. This binds its own handler to the same InputSelect action instead.
 *
 * It holds no lookup data of its own: the province-ID texture and table live on the map, and the
 * pawn asks it (ARtsWorldMap::ResolveProvinceAt) rather than keeping a second reference to the same
 * assets. It decides nothing either — the id leaves through OnProvinceClicked and what a selection
 * means is settled far above, in the HUD.
 *
 * Everything else — camera, movement, edge scrolling, frustum clamping — is inherited unchanged.
 */
UCLASS()
class WORLDMAP_API ARtsMapPawn : public ABA_MapPawn
{
	GENERATED_BODY()

public:
	ARtsMapPawn();

	/** Fires for every click, with an invalid id when the player clicked off any province. */
	UPROPERTY(BlueprintAssignable, Category="Province Lookup")
	FRtsMapProvinceClicked OnProvinceClicked;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void HandleSelect();
};
