#include "RtsHUD/RtsHUDViewModel.h"

URtsHUDViewModel::URtsHUDViewModel()
{

}

void URtsHUDViewModel::SetControlledCharacterId(FCharacterId InControlledCharacterId)
{
	UE_MVVM_SET_PROPERTY_VALUE(ControlledCharacterId, InControlledCharacterId);
}

void URtsHUDViewModel::SetActiveModal(EPlayerModal InActiveModal)
{
	UE_MVVM_SET_PROPERTY_VALUE(ActiveModal, InActiveModal);
}

void URtsHUDViewModel::SetSelectedPositionIndex(int32 InSelectedPositionIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, InSelectedPositionIndex);
}