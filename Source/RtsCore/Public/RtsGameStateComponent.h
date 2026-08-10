#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

/** Finds the game state component of type T for InContextObject's world. Nullptr outside a world
 * or before the game state has spawned yet. */
template<typename ComponentType>
ComponentType* FindRtsGameStateComponent(const UObject* InContextObject)
{
	if (InContextObject == nullptr || GEngine == nullptr)
	{
		return nullptr;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(InContextObject, EGetWorldErrorMode::ReturnNull);
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	return GameState ? GameState->FindComponentByClass<ComponentType>() : nullptr;
}
