#pragma once

#include "CoreMinimal.h"
#include "BA_MapPawn.h"
#include "RtsIds.h"
#include "RtsMapPawn.generated.h"

class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRtsMapProvinceClicked, FProvinceId, ProvinceId);

/**
 * The map camera pawn, and the only place a click on the world becomes a province.
 *
 * Exists for two reasons. ABA_MapPawn::Select is a BlueprintImplementableEvent, so the plugin's
 * click can only be picked up in an event graph — and a graph is exactly where this codebase does
 * not put logic; this binds its own handler to the same InputSelect action instead. And reading a
 * pixel needs the texture object, which is why the province-ID texture is referenced here, on the
 * class that does the reading, rather than on the map actor that only paints.
 *
 * It decides nothing beyond "which province was that": the id leaves through OnProvinceClicked and
 * what a selection means is settled far above, in the HUD.
 *
 * Everything else — camera, movement, edge scrolling, frustum clamping — is inherited unchanged.
 */
UCLASS()
class WORLDMAP_API ARtsMapPawn : public ABA_MapPawn
{
	GENERATED_BODY()

public:
	ARtsMapPawn();

	/** The same province-ID texture the plugin's map material samples. Must stay CPU-readable
	 * (uncompressed, no mips, no streaming) — resolving a click reads its pixels directly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Province Lookup")
	TObjectPtr<UTexture2D> ProvinceIdTexture;

	/** Fires for every click, with an invalid id when the player clicked off any province. */
	UPROPERTY(BlueprintAssignable, Category="Province Lookup")
	FRtsMapProvinceClicked OnProvinceClicked;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void HandleSelect();

	/** UV -> province-ID pixel -> UProvinces::FindByMapPixelColor. Invalid when nothing is under the
	 * cursor or no province claims that color. */
	FProvinceId ResolveProvinceAt(FVector2D InUV) const;
};
