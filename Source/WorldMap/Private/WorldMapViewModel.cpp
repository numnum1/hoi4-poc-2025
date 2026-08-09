#include "WorldMapViewModel.h"

void UWorldMapViewModel::SelectProvince(const FProvinceId InProvinceId)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceId, InProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedProvinceIdValue, InProvinceId.Value);
}
