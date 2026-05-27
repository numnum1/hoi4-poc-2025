// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RtsGameModeBase.generated.h"

class UScenario;

UCLASS()
class RTS_API ARtsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ARtsGameModeBase();

private:
	/**
	 * For Testing
	 */
	UPROPERTY(EditDefaultsOnly)
	UScenario* ScenarioToLoad;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void LoadScenario(UScenario* InScenario);
	
};
