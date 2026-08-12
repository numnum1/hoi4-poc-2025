#include "WorldMapWidget.h"

#include "Kismet/GameplayStatics.h"
#include "RtsMapPawn.h"
#include "RtsWorldMap.h"

void UWorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MapPawn = Cast<ARtsMapPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MapPawn)
	{
		MapPawn->OnProvinceClicked.AddDynamic(this, &UWorldMapWidget::HandleProvinceClicked);
	}
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
