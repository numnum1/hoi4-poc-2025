#include "RtsMapPawn.h"

#include "BA_DataFunctions.h"
#include "EnhancedInputComponent.h"
#include "Engine/Texture2D.h"
#include "Provinces.h"
#include "RtsGameStateComponent.h"
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
	// TraceUnderCursor dereferences MapReference without checking it, so it is not safe to call
	// before the pawn has been possessed and found the map.
	if (MapReference == nullptr)
	{
		MapReference = ARtsWorldMap::GetRtsWorldMap(this);
		if (MapReference == nullptr)
		{
			return;
		}
	}

	FHitResult HitResult;
	if (!TraceUnderCursor(HitResult))
	{
		OnProvinceClicked.Broadcast(FProvinceId());
		return;
	}

	OnProvinceClicked.Broadcast(ResolveProvinceAt(MapReference->VectorToUV(HitResult.ImpactPoint)));
}

FProvinceId ARtsMapPawn::ResolveProvinceAt(const FVector2D InUV) const
{
	const UProvinces* Provinces = FindRtsGameStateComponent<UProvinces>(this);
	if (ProvinceIdTexture == nullptr || Provinces == nullptr)
	{
		return FProvinceId();
	}

	const int32 Width = ProvinceIdTexture->GetSizeX();
	const int32 Height = ProvinceIdTexture->GetSizeY();
	const int32 X = FMath::Clamp(FMath::FloorToInt(InUV.X * Width), 0, Width - 1);
	const int32 Y = FMath::Clamp(FMath::FloorToInt(InUV.Y * Height), 0, Height - 1);

	FColor PixelColor;
	UBA_DataFunctions::FindColor(ProvinceIdTexture, X, Y, PixelColor);

	return Provinces->FindByMapPixelColor(PixelColor);
}
