#include "WorldMap/WorldMapWidget.h"

#include "PlayedCharacterView/PlayerCharacterViewModel.h"
#include "WorldMapViewModel.h"

void UWorldMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (WorldMapViewModel == nullptr)
	{
		WorldMapViewModel = NewObject<UWorldMapViewModel>(this);
	}
}

void UWorldMapWidget::SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel)
{
	PlayerViewModel = InPlayerViewModel;
}

void UWorldMapWidget::HandleProvinceClicked(const FProvinceId InProvinceId)
{
	if (WorldMapViewModel)
	{
		WorldMapViewModel->SelectProvince(InProvinceId);
	}

	if (PlayerViewModel)
	{
		PlayerViewModel->SelectProvince(InProvinceId);
	}
}
