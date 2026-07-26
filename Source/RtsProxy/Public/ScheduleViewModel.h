#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Schedule.h"
#include "MVVMViewModelBase.h"
#include "ScheduleViewModel.generated.h"

class UScheduleProxy;

/**
 * A read-only C++ MVVM view model for a single authored schedule from USchedules' library,
 * addressed by FScheduleId. Follows the pattern set by UCharacterViewModel: Initialize(Id) takes
 * a single parameter so it stays usable as an MVVM function binding target and fetches the
 * matching UScheduleProxy from URtsProxySubsystem.
 *
 * Unlike the other view models there is nothing to subscribe to: USchedules is filled once at
 * scenario load and is read-only afterwards, so Refresh only ever runs from Initialize and
 * UScheduleProxy has no events to offer.
 *
 * FSchedule is taken apart into one property per hour, the same way UCharacterViewModel exposes
 * a character's own schedule — UMG binds single values, not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSPROXY_API UScheduleViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Schedule Viewmodel")
	void Initialize(FScheduleId InScheduleId);

	/** The schedule entry for one hour of the day, 0-23. Out-of-range hours return SET_NONE. */
	UFUNCTION(BlueprintPure, Category="Schedule Viewmodel")
	EScheduleEntryType GetScheduleAtHour(int32 InHour) const;

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False when the id names no entry in the library. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Schedule Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel", meta=(AllowPrivateAccess))
	FScheduleId ScheduleId;

	/** ScheduleId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel", meta=(AllowPrivateAccess))
	int32 ScheduleIdValue = INDEX_NONE;

	//~ FScheduleInfo ------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Display", meta=(AllowPrivateAccess))
	FText ScheduleName;

	//~ FSchedule --------------------------------------------------------------------------------
	// One property per hour so a 24-slot day view can bind each slot directly. GetScheduleAtHour
	// covers the loop-driven case.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour00 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour01 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour02 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour03 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour04 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour05 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour06 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour07 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour08 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour09 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour10 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour11 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour12 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour13 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour14 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour15 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour16 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour17 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour18 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour19 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour20 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour21 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour22 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Hours", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour23 = EScheduleEntryType::SET_NONE;

public:
	bool IsValid() const { return bIsValid; }
	FScheduleId GetScheduleId() const { return ScheduleId; }
	int32 GetScheduleIdValue() const { return ScheduleIdValue; }

	FText GetScheduleName() const { return ScheduleName; }

	EScheduleEntryType GetScheduleHour00() const { return ScheduleHour00; }
	EScheduleEntryType GetScheduleHour01() const { return ScheduleHour01; }
	EScheduleEntryType GetScheduleHour02() const { return ScheduleHour02; }
	EScheduleEntryType GetScheduleHour03() const { return ScheduleHour03; }
	EScheduleEntryType GetScheduleHour04() const { return ScheduleHour04; }
	EScheduleEntryType GetScheduleHour05() const { return ScheduleHour05; }
	EScheduleEntryType GetScheduleHour06() const { return ScheduleHour06; }
	EScheduleEntryType GetScheduleHour07() const { return ScheduleHour07; }
	EScheduleEntryType GetScheduleHour08() const { return ScheduleHour08; }
	EScheduleEntryType GetScheduleHour09() const { return ScheduleHour09; }
	EScheduleEntryType GetScheduleHour10() const { return ScheduleHour10; }
	EScheduleEntryType GetScheduleHour11() const { return ScheduleHour11; }
	EScheduleEntryType GetScheduleHour12() const { return ScheduleHour12; }
	EScheduleEntryType GetScheduleHour13() const { return ScheduleHour13; }
	EScheduleEntryType GetScheduleHour14() const { return ScheduleHour14; }
	EScheduleEntryType GetScheduleHour15() const { return ScheduleHour15; }
	EScheduleEntryType GetScheduleHour16() const { return ScheduleHour16; }
	EScheduleEntryType GetScheduleHour17() const { return ScheduleHour17; }
	EScheduleEntryType GetScheduleHour18() const { return ScheduleHour18; }
	EScheduleEntryType GetScheduleHour19() const { return ScheduleHour19; }
	EScheduleEntryType GetScheduleHour20() const { return ScheduleHour20; }
	EScheduleEntryType GetScheduleHour21() const { return ScheduleHour21; }
	EScheduleEntryType GetScheduleHour22() const { return ScheduleHour22; }
	EScheduleEntryType GetScheduleHour23() const { return ScheduleHour23; }

private:
	void Refresh();

	UPROPERTY()
	TObjectPtr<UScheduleProxy> Proxy;
};
