#include "WorldMapWidget.h"

#include "WorldMapViewModel.h"

void UWorldMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (WorldMapViewModel == nullptr)
	{
		WorldMapViewModel = NewObject<UWorldMapViewModel>(this);
	}
}

void UWorldMapWidget::HandleProvinceClicked(const FProvinceId InProvinceId)
{
	if (WorldMapViewModel)
	{
		WorldMapViewModel->SelectProvince(InProvinceId);
	}
}
