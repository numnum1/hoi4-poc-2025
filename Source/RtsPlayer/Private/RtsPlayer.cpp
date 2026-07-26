#include "RtsPlayer.h"

#include "Characters.h"
#include "GameFramework/GameStateBase.h"

#define LOCTEXT_NAMESPACE "FRtsPlayerModule"

void FRtsPlayerModule::StartupModule()
{
    
}

void FRtsPlayerModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRtsPlayerModule, RtsPlayer)

ARtsPlayer::ARtsPlayer()
{
}

void ARtsPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (const AGameStateBase* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr)
	{
		if (UCharacters* Characters = GameState->FindComponentByClass<UCharacters>())
		{
			Characters->OnCharacterDestroyed.AddDynamic(this, &ARtsPlayer::HandleControlledCharacterDestroyed);
		}
	}
}

void ARtsPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const AGameStateBase* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr)
	{
		if (UCharacters* Characters = GameState->FindComponentByClass<UCharacters>())
		{
			Characters->OnCharacterDestroyed.RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ARtsPlayer::HandleControlledCharacterDestroyed(const FCharacterId InCharacterId)
{
	if (InCharacterId == ControlledCharacterId)
	{
		ControlledCharacterId = FCharacterId();
		OnControlledCharacterIdChanged.Broadcast();
	}
}
