#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RtsIds.h"
#include "RtsMapPawn.generated.h"

class ARtsWorldMap;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRtsMapProvinceClicked, FProvinceId, ProvinceId);

/**
 * The map camera pawn, and where a click on the world becomes a province.
 *
 * Moved out of the BloodAnvil plugin's ABA_MapPawn for the same reason as ARtsWorldMap — Plugins/ is
 * not in this repository — and because the plugin only offered its click as a
 * BlueprintImplementableEvent, so it could be picked up in an event graph and nowhere else. Here the
 * click is bound natively and leaves through OnProvinceClicked.
 *
 * Camera movement is still driven from the pawn Blueprint's tick through AddMovement, which is why
 * that function and its settings keep their names: the plugin's own Tick was dead code with its
 * whole body commented out, so this has always been the arrangement.
 *
 * It holds no lookup data of its own: the province-ID texture and table live on the map, and the
 * pawn asks it (ARtsWorldMap::ResolveProvinceAt) rather than keeping a second reference to the same
 * assets. It decides nothing either — what a selection means is settled far above, in the HUD.
 */
UCLASS()
class WORLDMAP_API ARtsMapPawn : public APawn
{
	GENERATED_BODY()

public:
	ARtsMapPawn();

	/** Fires for every click, with an invalid id when the player clicked off any province. */
	UPROPERTY(BlueprintAssignable, Category="Province Lookup")
	FRtsMapProvinceClicked OnProvinceClicked;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	/** Traces from the cursor onto the map mesh. Wraps nothing up: the caller decides what a hit
	 * means. */
	UFUNCTION(BlueprintCallable, Category="Tracing")
	bool TraceUnderCursor(FHitResult& OutHitResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tracing")
	bool DebugTraces = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tracing")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(BlueprintReadOnly, Category="Map Data")
	TObjectPtr<ARtsWorldMap> MapReference;

	UPROPERTY(BlueprintReadOnly, Category="Player Reference")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	bool DontScrollOnSelect = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> InputMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> InputSelect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings")
	FRotator StartRotationGlobe = FRotator(-48.0f, -95.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings")
	FVector2D StartLocationFlat = FVector2D(1250.0f, -12890.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings")
	float MoveSpeedGlobe = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings", meta=(Units="CentimetersPerSecond"))
	float MoveSpeedFlat = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings", meta=(Units="Centimeters"))
	float StartHeight = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings", meta=(Units="Centimeters"))
	FVector2D FrustumClampOffset = FVector2D(-10.0f, -10.0f);

	/** How much of the input translates into movement at the lowest and the highest altitude — the
	 * camera has to crawl when zoomed in and sweep when zoomed out. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings")
	float MinHeightRelativeMovementScale = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings")
	float MaxHeightRelativeMovementScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings", meta=(Units="Centimeters"))
	float MinHeightRelativeMovementDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings", meta=(Units="Centimeters"))
	float MaxHeightRelativeMovementDistance = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Height", meta=(Units="Centimeters"))
	float MinHeight = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Height", meta=(Units="Centimeters"))
	float MaxHeight = 20000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Height", meta=(Units="CentimetersPerSecond"))
	float HeightMoveSpeed = 30000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Height")
	float HeightInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Height", meta=(Units="Centimeters"))
	float MinHeightPitch = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Height", meta=(Units="Centimeters"))
	float MaxHeightPitch = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Height", meta=(Units="Degrees"))
	float HeightPitch = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings|Edge Scrolling")
	float EdgeMovementSpeed = 1.0f;

protected:
	UFUNCTION(BlueprintCallable, Category="Movement")
	float ComputeMovementScale(float InDeltaTime) const;

	/** Called by the pawn Blueprint's tick with the frame's input direction. */
	UFUNCTION(BlueprintCallable, Category="Movement")
	void AddMovement(float InDeltaTime, const FVector& InDirections);

	UFUNCTION(BlueprintCallable, Category="Movement")
	void ClampCameraPosition();

private:
	/** Frames the camera for the map's projection. Bound to ARtsWorldMap::MapTypeChanged. */
	UFUNCTION()
	void HandleMapTypeChanged(ARtsWorldMap* InMap);

	void HandleSelectStarted();
	void HandleSelectCompleted();

	/** Interp target for the spring arm, kept separate so zooming stays smooth. */
	float ArmLength = 0.0f;

	/** Whether the select button is held — the pawn drags the map instead of edge-scrolling while
	 * it is. */
	bool bSelectDown = false;
};
