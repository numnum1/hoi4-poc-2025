#include "WorldMapWidget.h"

#include "Kismet/GameplayStatics.h"
#include "RtsMapPawn.h"
#include "RtsWorldMap.h"

DEFINE_LOG_CATEGORY_STATIC(LogWorldMapWidget, Log, All);

void UWorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MapPawn = Cast<ARtsMapPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MapPawn == nullptr)
	{
		// Worth saying out loud: without the pawn the map still draws, it just silently swallows
		// every click, which looks like a broken selection rather than a missing binding.
		UE_LOG(LogWorldMapWidget, Error,
			TEXT("%s found no ARtsMapPawn to take province clicks from — check that the possessed "
				 "pawn derives from ARtsMapPawn."), *GetName());
		return;
	}

	MapPawn->OnProvinceClicked.AddDynamic(this, &UWorldMapWidget::HandleProvinceClicked);
}

void UWorldMapWidget::NativeDestruct()
{
	if (MapPawn)
	{
		MapPawn->OnProvinceClicked.RemoveAll(this);
		MapPawn = nullptr;
	}

	Super::NativeDestruct();
}

void UWorldMapWidget::SetProvinceVisuals(const TMap<FProvinceId, FProvinceVisuals>& InVisuals)
{
	if (ARtsWorldMap* WorldMap = ARtsWorldMap::GetRtsWorldMap(this))
	{
		WorldMap->SetProvinceVisuals(InVisuals);
	}
}

void UWorldMapWidget::HandleProvinceClicked(const FProvinceId InProvinceId)
{
	OnProvinceClicked.Broadcast(InProvinceId);
}
