// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RtsGameState.generated.h"

class UProvinces;
class UNations;
class UCharacters;
/**
 * 
 */
UCLASS()
class RTS_API ARtsGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ARtsGameState();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCharacters* CharactersContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UNations* NationsContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UProvinces* ProvincesContainer;
};
