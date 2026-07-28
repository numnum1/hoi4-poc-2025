#include "PlayerCharacterViewModel.h"

#include "CharacterActionsViewModel.h"
#include "RtsPlayer.h"

void UPlayerCharacterViewModel::InitializeForPlayer()
{
	ARtsPlayer* NewPlayer = GetWorld() ? Cast<ARtsPlayer>(GetWorld()->GetFirstPlayerController()) : nullptr;
	if (Player == NewPlayer)
	{
		return;
	}

	if (Player)
	{
		Player->OnControlledCharacterIdChanged.RemoveAll(this);
	}

	Player = NewPlayer;

	// Created once and reused: Refresh_Implementation re-initializes it with the current
	// character every time, it does not need to be rebuilt.
	if (Actions == nullptr)
	{
		Actions = NewObject<UCharacterActionsViewModel>(this);
	}

	if (Player)
	{
		Player->OnControlledCharacterIdChanged.AddDynamic(this, &UPlayerCharacterViewModel::HandleControlledCharacterChanged);
	}

	HandleControlledCharacterChanged();
}

void UPlayerCharacterViewModel::HandleControlledCharacterChanged()
{
	// Inherited from UCharacterViewModel — resolves the proxy, rebinds OnChanged, and calls
	// Refresh(), which our override below also drives Actions and the selection from.
	Initialize(Player ? Player->GetControlledCharacterId() : FCharacterId());
}

void UPlayerCharacterViewModel::Refresh_Implementation()
{
	Super::Refresh_Implementation();

	if (Proxy == nullptr)
	{
		UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, INDEX_NONE);
		UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, true);
		return;
	}

	// A position the character no longer holds (or never held, on the first refresh) falls back
	// to acting as self rather than pointing at a stale index.
	if (SelectedPositionIndex != INDEX_NONE && !GetPositions().IsValidIndex(SelectedPositionIndex))
	{
		UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, INDEX_NONE);
		UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, true);
	}

	if (Actions)
	{
		Actions->Initialize(GetCharacterId());
	}

	ApplySelection();
}

void UPlayerCharacterViewModel::SelectSelf()
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, true);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, INDEX_NONE);
	ApplySelection();
}

void UPlayerCharacterViewModel::SelectPosition(const int32 InPositionIndex)
{
	if (!GetPositions().IsValidIndex(InPositionIndex))
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsActingAsSelf, false);
	UE_MVVM_SET_PROPERTY_VALUE(SelectedPositionIndex, InPositionIndex);
	ApplySelection();
}

void UPlayerCharacterViewModel::ApplySelection()
{
	if (Actions == nullptr)
	{
		return;
	}

	if (bIsActingAsSelf)
	{
		Actions->SetScopeSelf();
	}
	else
	{
		Actions->SetScopePosition(SelectedPositionIndex);
	}
}

void UPlayerCharacterViewModel::BeginDestroy()
{
	if (Player)
	{
		Player->OnControlledCharacterIdChanged.RemoveAll(this);
	}
	Super::BeginDestroy();
}
