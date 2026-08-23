#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RtsMapPawn.generated.h"

class ARtsWorldMap;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;

/** Fired on a right click anywhere over the map, regardless of what it hit — unlike the left click,
 * nothing here resolves a province, so there is nothing to pass along. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMapPawnRightClicked);

/**
 * The map camera pawn: a camera framed on the globe, and the trace that turns a cursor position
 * into a point on it.
 *
 * Moved out of the BloodAnvil plugin's ABA_MapPawn for the same reason as ARtsWorldMap — Plugins/ is
 * not in this repository — and because the plugin only offered its click as a
 * BlueprintImplementableEvent, so it could be picked up in an event graph and nowhere else.
 *
 * Camera movement is still driven from the pawn Blueprint's tick through AddMovement, which is why
 * that function and its settings keep their names: the plugin's own Tick was dead code with its
 * whole body commented out, so this has always been the arrangement.
 *
 * It resolves nothing. A click leaves as ARtsWorldMap::NotifyWorldClicked and the map decides what
 * was hit, so the pawn never touches a province id. The dependency runs this way only — WorldMap
 * must not depend on MapCamera, or the two modules would be a cycle.
 */
UCLASS()
class MAPCAMERA_API ARtsMapPawn : public APawn
{
	GENERATED_BODY()

public:
	ARtsMapPawn();

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

	/** Broadcast from HandleRightClickStarted. Bound by UPlayerCharacterViewModel to clear the
	 * current province selection. */
	UPROPERTY(BlueprintAssignable, Category="Tracing")
	FMapPawnRightClicked OnRightMouseButtonClicked;

	UPROPERTY(BlueprintReadOnly, Category="Map Data")
	TObjectPtr<ARtsWorldMap> MapReference;

	UPROPERTY(BlueprintReadOnly, Category="Player Reference")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> InputMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> InputSelect;

	/** No default asset finder for this one — assign the Input Action in the pawn Blueprint's
	 * defaults, the same way InputMapping/InputMove/InputSelect would if the constructor's
	 * ConstructorHelpers lookups ever missed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> InputRightClick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings")
	FRotator StartRotationGlobe = FRotator(-50.0f, -100.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings")
	float MoveSpeedGlobe = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement Settings", meta=(Units="Centimeters"))
	float StartHeight = 5000.0f;

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

protected:
	UFUNCTION(BlueprintCallable, Category="Movement")
	float ComputeMovementScale(float InDeltaTime) const;

	/** Called by the pawn Blueprint's tick with the frame's input direction. */
	UFUNCTION(BlueprintCallable, Category="Movement")
	void AddMovement(float InDeltaTime, const FVector& InDirections);

private:
	/** Frames the camera on the globe at its starting height. Called once the pawn has a map and a
	 * controller to frame it for. */
	void FrameCamera();

	void HandleSelectStarted();

	/** Broadcasts OnRightMouseButtonClicked. Resolves nothing itself, same as the pawn's class
	 * comment says about the left click — a listener decides what a right click means. */
	void HandleRightClickStarted();

	/** Interp target for the spring arm, kept separate so zooming stays smooth. */
	float ArmLength = 0.0f;
};
