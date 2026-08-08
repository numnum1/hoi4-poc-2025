#include "Organizations/MemberViewModel.h"

void UMemberViewModel::SetSource(const FCharacterId InCharacterId, const FText& InCharacterName,
                                  const FSlateBrush& InPortrait, const FText& InRoleName)
{
	CharacterId = InCharacterId;
	UE_MVVM_SET_PROPERTY_VALUE(CharacterName, InCharacterName);
	UE_MVVM_SET_PROPERTY_VALUE(Portrait, InPortrait);
	UE_MVVM_SET_PROPERTY_VALUE(RoleName, InRoleName);
}

void UMemberViewModel::CallOnClicked()
{
	OnClicked.Broadcast(this);
}
