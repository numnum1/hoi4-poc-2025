#include "SelectableCharacterViewModel.h"

void USelectableCharacterViewModel::SetSource(const FCharacterId InCharacterId, const FText& InDisplayName,
                                               const FSlateBrush& InPortrait)
{
	CharacterId = InCharacterId;
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, InDisplayName);
	UE_MVVM_SET_PROPERTY_VALUE(Portrait, InPortrait);
}

void USelectableCharacterViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
