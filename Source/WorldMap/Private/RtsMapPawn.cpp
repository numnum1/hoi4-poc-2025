#include "RtsMapPawn.h"

#include "EnhancedInputComponent.h"
#include "RtsWorldMap.h"

ARtsMapPawn::ARtsMapPawn()
{
	TraceChannel = ECollisionChannel::ECC_Visibility;
}

void ARtsMapPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Super sets up PlayerController, the mapping context and the plugin's own bindings; without it
	// TraceUnderCursor has no controller to deproject through.
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr || InputSelect == nullptr)
	{
		return;
	}

	EnhancedInputComponent->BindAction(InputSelect, ETriggerEvent::Started, this, &ARtsMapPawn::HandleSelect);
}

void ARtsMapPawn::HandleSelect()
{
	ARtsWorldMap* WorldMap = ARtsWorldMap::GetRtsWorldMap(this);
	if (WorldMap == nullptr)
	{
		return;
	}

	// TraceUnderCursor dereferences MapReference without checking it, so it is not safe to call
	// before the pawn has been possessed and found the map.
	if (MapReference == nullptr)
	{
		MapReference = WorldMap;
	}

	FHitResult HitResult;
	if (!TraceUnderCursor(HitResult))
	{
		OnProvinceClicked.Broadcast(FProvinceId());
		return;
	}

	OnProvinceClicked.Broadcast(WorldMap->ResolveProvinceAt(WorldMap->VectorToUV(HitResult.ImpactPoint)));
}
