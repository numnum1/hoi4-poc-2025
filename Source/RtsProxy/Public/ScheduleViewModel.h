#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Schedule.h"
#include "MVVMViewModelBase.h"
#include "ScheduleViewModel.generated.h"

class UCharacterProxy;

/**
 * A read-only C++ MVVM view model for a single character's actual running schedule, addressed by
 * FCharacterId rather than FScheduleId: the hours shown here are the character's own copy (which
 * can diverge from the library entry it was copied from) and displaying them meaningfully also
 * needs the character's current timezone offset, so both have to come from the same character.
 * Follows the pattern set by UCharacterViewModel: Initialize(Id) takes a single parameter so it
 * stays usable as an MVVM function binding target, fetches the matching UCharacterProxy from
 * URtsProxySubsystem, and refreshes flat FieldNotify properties whenever that proxy reports a
 * change — a moving character can end up in a different timezone without its schedule changing.
 *
 * ScheduleId/ScheduleName still identify the USchedules library entry the running schedule was
 * copied from, read through UCharacterProxy::GetScheduleTemplate, purely for display.
 *
 * FSchedule is taken apart into one property per hour, the same way UCharacterViewModel exposes
 * a character's own schedule — UMG binds single values, not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSPROXY_API UScheduleViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** The schedule entry for one hour of the day, 0-23. Out-of-range hours return SET_NONE. */
	UFUNCTION(BlueprintPure, Category="Schedule Viewmodel")
	EScheduleEntryType GetScheduleAtHour(int32 InHour) const;

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False once the character's entity has been destroyed. Display keeps its last known values. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Schedule Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel", meta=(AllowPrivateAccess))
	FCharacterId CharacterId;

	/** CharacterId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel", meta=(AllowPrivateAccess))
	int32 CharacterIdValue = INDEX_NONE;

	//~ Schedule template, for display only -------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Display", meta=(AllowPrivateAccess))
	FScheduleId ScheduleId;

	/** ScheduleId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Display", meta=(AllowPrivateAccess))
	int32 ScheduleIdValue = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel|Display", meta=(AllowPrivateAccess))
	FText ScheduleName;

	//~ Timezone ---------------------------------------------------------------------------------

	/** Hours offset from game time for the character's current province, read through its proxy. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Schedule Viewmodel", meta=(AllowPrivateAccess))
	int32 TimezoneOffset = 0;

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
	FCharacterId GetCharacterId() const { return CharacterId; }
	int32 GetCharacterIdValue() const { return CharacterIdValue; }

	FScheduleId GetScheduleId() const { return ScheduleId; }
	int32 GetScheduleIdValue() const { return ScheduleIdValue; }
	FText GetScheduleName() const { return ScheduleName; }

	int32 GetTimezoneOffset() const { return TimezoneOffset; }

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

public:
	UFUNCTION(BlueprintCallable, Category="Schedule Viewmodel")
	void Initialize(FCharacterId InCharacterId);
protected:
	UFUNCTION(BlueprintNativeEvent, Category="Schedule Viewmodel")
	void Refresh();

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Unbind();

	UFUNCTION()
	void HandleProxyChanged();

	UPROPERTY()
	TObjectPtr<UCharacterProxy> Proxy;
};
