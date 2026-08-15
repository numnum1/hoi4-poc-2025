#include "RtsMapPawn.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "RtsWorldMap.h"
#include "UObject/ConstructorHelpers.h"
#include "UnrealClient.h"

ARtsMapPawn::ARtsMapPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 7.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.0f;
	SetRootComponent(SpringArm);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingFinder(TEXT("/BloodAnvil/Editors/Shared/Input/IMC_DefaultPlayer.IMC_DefaultPlayer"));
	if (InputMappingFinder.Succeeded())
	{
		InputMapping = InputMappingFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputMoveFinder(TEXT("/BloodAnvil/Editors/Shared/Input/Move.Move"));
	if (InputMoveFinder.Succeeded())
	{
		InputMove = InputMoveFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputSelectFinder(TEXT("/BloodAnvil/Editors/Shared/Input/Select.Select"));
	if (InputSelectFinder.Succeeded())
	{
		InputSelect = InputSelectFinder.Object;
	}
}

void ARtsMapPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	MapReference = ARtsWorldMap::GetRtsWorldMap(this);
	if (MapReference == nullptr)
	{
		return;
	}

	FrameCamera();
}

void ARtsMapPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
	{
		return;
	}

	PlayerController->bShowMouseCursor = true;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	InputModeData.SetHideCursorDuringCapture(false);
	PlayerController->SetInputMode(InputModeData);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (Subsystem == nullptr || EnhancedInputComponent == nullptr || InputSelect == nullptr)
	{
		return;
	}

	Subsystem->AddMappingContext(InputMapping, 0);
	EnhancedInputComponent->BindAction(InputSelect, ETriggerEvent::Started, this, &ARtsMapPawn::HandleSelectStarted);
}

bool ARtsMapPawn::TraceUnderCursor(FHitResult& OutHitResult)
{
	if (PlayerController == nullptr || MapReference == nullptr)
	{
		return false;
	}

	FVector MousePosition;
	FVector MouseDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(MousePosition, MouseDirection))
	{
		return false;
	}

	const FVector Start = MousePosition;
	// The arm length is already the distance to the globe's surface.
	const FVector End = MouseDirection * SpringArm->TargetArmLength + Start;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, TraceChannel);

#if WITH_EDITOR
	if (DebugTraces)
	{
		DrawDebugLine(GetWorld(), Start, bHit ? OutHitResult.ImpactPoint : End, FColor::Red, false, 10.0f);
		if (bHit)
		{
			DrawDebugPoint(GetWorld(), OutHitResult.ImpactPoint, 10.0f, FColor::Red, false, 10.0f);
		}
	}
#endif

	return bHit;
}

float ARtsMapPawn::ComputeMovementScale(const float InDeltaTime) const
{
	return FMath::GetMappedRangeValueClamped(
		FVector2D(MinHeightRelativeMovementDistance, MaxHeightRelativeMovementDistance),
		FVector2D(MinHeightRelativeMovementScale, MaxHeightRelativeMovementScale),
		ArmLength) * InDeltaTime;
}

void ARtsMapPawn::AddMovement(const float InDeltaTime, const FVector& InDirections)
{
	if (MapReference == nullptr)
	{
		return;
	}

	const float MovementScale = FMath::GetMappedRangeValueClamped(
		FVector2D(MinHeightRelativeMovementDistance, MaxHeightRelativeMovementDistance),
		FVector2D(MinHeightRelativeMovementScale, MaxHeightRelativeMovementScale),
		ArmLength - MapReference->MapRadius) * InDeltaTime;

	// Moving over a globe is rotating the camera around it, not translating it.
	AddControllerYawInput(-InDirections.X * MovementScale * MoveSpeedGlobe);
	AddControllerPitchInput(-InDirections.Y * MovementScale * MoveSpeedGlobe);

	ArmLength = FMath::Clamp(ArmLength + InDirections.Z * HeightMoveSpeed * MovementScale,
		MinHeight + MapReference->MapRadius, MaxHeight + MapReference->MapRadius);
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLength, InDeltaTime, HeightInterpSpeed);
	Camera->SetRelativeRotation(FRotator(
		FMath::GetMappedRangeValueClamped(FVector2D(MinHeightPitch, MaxHeightPitch), FVector2D(HeightPitch, 0.0f),
			SpringArm->TargetArmLength - MapReference->MapRadius),
		Camera->GetRelativeRotation().Yaw, Camera->GetRelativeRotation().Roll));
}

void ARtsMapPawn::FrameCamera()
{
	if (MapReference == nullptr || !IsLocallyControlled() || GetController() == nullptr)
	{
		return;
	}

	GetController()->SetControlRotation(StartRotationGlobe);
	SetActorLocation(FVector::ZeroVector);
	SpringArm->TargetArmLength = StartHeight + MapReference->MapRadius;
	ArmLength = SpringArm->TargetArmLength;

	// Ticked by hand with lag off, or the camera would visibly glide in from wherever it was framed
	// before instead of simply starting there.
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->TickComponent(0.0f, ELevelTick::LEVELTICK_PauseTick, nullptr);
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
}

void ARtsMapPawn::HandleSelectStarted()
{
	if (MapReference == nullptr)
	{
		MapReference = ARtsWorldMap::GetRtsWorldMap(this);
	}
	if (MapReference == nullptr)
	{
		return;
	}

	FHitResult HitResult;
	if (!TraceUnderCursor(HitResult))
	{
		OnProvinceClicked.Broadcast(FProvinceId());
		return;
	}

	OnProvinceClicked.Broadcast(MapReference->ResolveProvinceAt(MapReference->VectorToUV(HitResult.ImpactPoint)));
}
