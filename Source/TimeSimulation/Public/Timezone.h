// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Timezone.generated.h"

class ATimeSimulation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimezoneEvent, int32, InHoursPassed);

UCLASS()
class TIMESIMULATION_API ATimezone : public AActor
{
	GENERATED_BODY()

public:
	ATimezone();

	UPROPERTY(BlueprintAssignable)
	FTimezoneEvent OnHoursPassed;
	UPROPERTY(BlueprintAssignable)
	FTimezoneEvent OnDaysPassed;
	UPROPERTY(BlueprintAssignable)
	FTimezoneEvent OnMonthsPassed;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	ATimeSimulation* TimeSimulation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 Offset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FDateTime ActualDateTime;
	
	UFUNCTION()
	void Update();
};
