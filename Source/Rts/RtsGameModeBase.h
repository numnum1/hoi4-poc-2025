// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingCatalog.h"
#include "GameFramework/GameModeBase.h"
#include "RtsGameModeBase.generated.h"

class URaces;
class UScenario;

UCLASS()
class RTS_API ARtsGameModeBase : public AGameModeBase, public IBuildingCatalog
{
	GENERATED_BODY()
public:
	ARtsGameModeBase();

	virtual const TArray<TObjectPtr<UBuildingType>>& GetBuildingTypes() const override { return BuildingTypes; }

	FORCEINLINE URaces* GetRacesContainer() const { return RacesContainer; }

private:
	/**
	 * For Testing
	 */
	UPROPERTY(EditDefaultsOnly)
	UScenario* ScenarioToLoad;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBuildingType>> BuildingTypes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<URaces> RacesContainer;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void LoadScenario(UScenario* InScenario);
	
};
