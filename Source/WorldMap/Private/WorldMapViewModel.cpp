#include "WorldMapViewModel.h"

void UWorldMapViewModel::SetOverriddenVisuals(const TMap<FProvinceId, FProvinceVisuals>& InOverriddenVisuals)
{
	// TMap deletes operator==, so UE_MVVM_SET_PROPERTY_VALUE cannot do this comparison — and it has
	// to happen here: every broadcast redraws the map's LUT render targets.
	if (OverriddenVisuals.OrderIndependentCompareEqual(InOverriddenVisuals))
	{
		return;
	}

	OverriddenVisuals = InOverriddenVisuals;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(OverriddenVisuals);
}
