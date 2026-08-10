#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "TimezoneViewModel.generated.h"

class UCharacters;
class UProvinces;
class UTime;

/**
 * The clock as one character experiences it: game time shifted by the timezone offset of the
 * province that character currently stands in, plus the running speed.
 *
 * Addressed by FCharacterId like UScheduleViewModel is, and for the same reason — the offset is a
 * property of where the character *is*, so both the province lookup and the shift have to come
 * from the same character. Initialize(Id) therefore takes a single parameter and stays usable as
 * an MVVM function binding target; the facades (UCharacters for the province, UProvinces for its
 * offset, UTime for the clock) are resolved off the game state. Two independent sources
 * change what is shown here and both are bound: the character moving into another timezone
 * (UCharacters::OnCurrentProvinceChanged) and the clock advancing (UTime::OnCurrentTimeChanged).
 *
 * UTime ticks several times a second, so Refresh runs far more often than anything visibly
 * changes. That costs nothing: every value goes through UE_MVVM_SET_PROPERTY_VALUE, which only
 * broadcasts on an actual change, and the displayed values are minute-precise.
 *
 * This is a CreateInstance view model, not a child object hanging off UPlayerCharacterViewModel:
 * the widget blueprint declares its own entry for it and drives Initialize from a binding on the
 * played character's id. Nothing owns it, so the speed buttons are answered here — the two
 * CallOn... functions set UTime's speed and that is the whole of it. There is deliberately no
 * dispatcher alongside them: a CreateInstance view model has no owner to report to, and anything
 * else that wants to know the speed binds Speed/bIsFastForward below. See "Callbacks gibt es nur
 * auf Manual-ViewModels" in CLAUDE.md. The new speed still comes back through UTime::OnSpeedChanged
 * rather than being written into Speed directly, so what is displayed is always what the clock
 * actually runs at.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UTimezoneViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Bind the fast-forward button's OnClicked to this. What "fast" is stays hardcoded — see
	 * "Keine Erweiterbarkeit". Keeps its CallOn... name because the Timezone widget's Blueprint
	 * binds it by name; it no longer broadcasts anything. */
	UFUNCTION(BlueprintCallable, Category="Timezone Viewmodel|Speed")
	void CallOnFastForwardClicked();

	UFUNCTION(BlueprintCallable, Category="Timezone Viewmodel|Speed")
	void CallOnNormalSpeedClicked();

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False while the facades are missing or the character's entity is gone. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	FCharacterId CharacterId;

	/** CharacterId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	int32 CharacterIdValue = INDEX_NONE;

	//~ Timezone ---------------------------------------------------------------------------------

	/** The province whose timezone this clock is showing — the character's current one. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	FProvinceId ProvinceId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	int32 ProvinceIdValue = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	FText ProvinceName;

	/** Hours added to game time to get local time, as the hourly sim shifts it. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	int32 TimezoneOffset = 0;

	/** The offset as a label, e.g. "UTC+2". */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel", meta=(AllowPrivateAccess))
	FText TimezoneOffsetText;

	//~ Local time -------------------------------------------------------------------------------
	// Flat, one property per part, because UMG binds single values and not an FDateTime.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Local Time", meta=(AllowPrivateAccess))
	int32 LocalHour = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Local Time", meta=(AllowPrivateAccess))
	int32 LocalMinute = 0;

	/** "HH:MM" in the character's timezone. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Local Time", meta=(AllowPrivateAccess))
	FText LocalTimeText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Local Time", meta=(AllowPrivateAccess))
	int32 LocalDay = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Local Time", meta=(AllowPrivateAccess))
	int32 LocalMonth = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Local Time", meta=(AllowPrivateAccess))
	int32 LocalYear = 0;

	/** "YYYY-MM-DD HH:MM" in the character's timezone — the full stamp, for a single-widget display.
	 * LocalTimeText is the same clock without the date. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Local Time", meta=(AllowPrivateAccess))
	FText LocalDateText;

	//~ Speed ------------------------------------------------------------------------------------
	// Shown, not owned: what the buttons above do about it is the owner's decision.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Timezone Viewmodel|Speed", meta=(AllowPrivateAccess))
	int32 Speed = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsFastForward, Category="Timezone Viewmodel|Speed", meta=(AllowPrivateAccess))
	bool bIsFastForward = false;

public:
	bool IsValid() const { return bIsValid; }
	FCharacterId GetCharacterId() const { return CharacterId; }
	int32 GetCharacterIdValue() const { return CharacterIdValue; }

	FProvinceId GetProvinceId() const { return ProvinceId; }
	int32 GetProvinceIdValue() const { return ProvinceIdValue; }
	FText GetProvinceName() const { return ProvinceName; }
	int32 GetTimezoneOffset() const { return TimezoneOffset; }
	FText GetTimezoneOffsetText() const { return TimezoneOffsetText; }

	int32 GetLocalHour() const { return LocalHour; }
	int32 GetLocalMinute() const { return LocalMinute; }
	FText GetLocalTimeText() const { return LocalTimeText; }
	int32 GetLocalDay() const { return LocalDay; }
	int32 GetLocalMonth() const { return LocalMonth; }
	int32 GetLocalYear() const { return LocalYear; }
	FText GetLocalDateText() const { return LocalDateText; }

	int32 GetSpeed() const { return Speed; }
	bool IsFastForward() const { return bIsFastForward; }

public:
	UFUNCTION(BlueprintCallable, Category="Timezone Viewmodel")
	void Initialize(FCharacterId InCharacterId);

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Timezone Viewmodel")
	void Refresh();

	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

private:
	void SetSpeed(int32 InSpeed);

	void Unbind();

	UFUNCTION()
	void HandleCharacterEvent(FCharacterId InCharacterId);

	UFUNCTION()
	void HandleTimeEvent();

	UPROPERTY()
	TObjectPtr<UCharacters> Characters;

	UPROPERTY()
	TObjectPtr<UProvinces> Provinces;

	UPROPERTY()
	TObjectPtr<UTime> Time;
};
