#include "MapModes/MapModeEntryViewModel.h"

#include "MapModes/RtsMapModes.h"

void UMapModeEntryViewModel::SetMapMode(const EMapMode InMapMode)
{
	MapMode = InMapMode;
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, URtsMapModes::GetDisplayName(InMapMode));
	UE_MVVM_SET_PROPERTY_VALUE(Icon, URtsMapModes::GetIcon(InMapMode));
}

void UMapModeEntryViewModel::SetActive(const bool bInActive)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsActive, bInActive);
}

void UMapModeEntryViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
