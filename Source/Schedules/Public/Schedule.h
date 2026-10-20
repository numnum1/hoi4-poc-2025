// Copyright (c) 2026 Marc Merten. Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Schedule.generated.h"

UENUM(BlueprintType)
enum class EScheduleEntryType : uint8
{
	SET_NONE UMETA(DisplayName = "None"),
	SET_SLEEP UMETA(DisplayName = "Sleep"),
	SET_WORK UMETA(DisplayName = "Work"),
	SET_LEISURE UMETA(DisplayName = "Leisure"),
	SET_STUDY UMETA(DisplayName = "Study"),
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct SCHEDULES_API FSchedule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Zero = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType One = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Two = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Three = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Four = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Five = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Six = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Seven = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Eight = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Nine = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Ten = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Eleven = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Twelve = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Thirteen = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Fourteen = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Fifteen = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Sixteen = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Seventeen = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Eighteen = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Nineteen = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType Twenty = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType TwentyOne = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType TwentyTwo = EScheduleEntryType::SET_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EScheduleEntryType TwentyThree = EScheduleEntryType::SET_NONE;
	
	FORCEINLINE EScheduleEntryType GetAtHour(const uint8 InHour) const
	{
		switch (InHour)
		{
		case 0:  return Zero;
		case 1:  return One;
		case 2:  return Two;
		case 3:  return Three;
		case 4:  return Four;
		case 5:  return Five;
		case 6:  return Six;
		case 7:  return Seven;
		case 8:  return Eight;
		case 9:  return Nine;
		case 10: return Ten;
		case 11: return Eleven;
		case 12: return Twelve;
		case 13: return Thirteen;
		case 14: return Fourteen;
		case 15: return Fifteen;
		case 16: return Sixteen;
		case 17: return Seventeen;
		case 18: return Eighteen;
		case 19: return Nineteen;
		case 20: return Twenty;
		case 21: return TwentyOne;
		case 22: return TwentyTwo;
		case 23: return TwentyThree;
		default: return EScheduleEntryType::SET_NONE;
		}
	}
	
	FORCEINLINE EScheduleEntryType operator[](const uint8 InHour) const
	{
		return GetAtHour(InHour);
	}
	
};
