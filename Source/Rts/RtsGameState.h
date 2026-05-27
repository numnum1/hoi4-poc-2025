// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RtsGameState.generated.h"

class UTime;
class URaces;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	URaces* RacesContainer;
	
	// Simulation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UTime* Time;
	
public:
	FORCEINLINE UCharacters* GetCharactersContainer() { return CharactersContainer; }
	FORCEINLINE UNations* GetNationsContainer() { return NationsContainer; }
	FORCEINLINE UProvinces* GetProvincesContainer() { return ProvincesContainer; }
	FORCEINLINE URaces* GetRacesContainer() { return RacesContainer; }
	
	FORCEINLINE UTime* GetTime() const { return Time; }
	
	// Utils
	UFUNCTION(BlueprintCallable)
	int32 GetTimezoneOffsetOfProvince(int32 InProvinceIndex) const;
	
};
