#pragma once

#include "CoreMinimal.h"
#include "CharacterInfo.h"
#include "RtsIds.h"
#include "Schedule.h"
#include "MVVMViewModelBase.h"
#include "CharacterViewModel.generated.h"

class UCharacterProxy;

/**
 * A read-only C++ MVVM view model for a single character, addressed by the same FCharacterId
 * every other character API uses. Call Initialize once (e.g. from a widget's construct event,
 * or as an MVVM function binding target — it takes a single FCharacterId parameter so it is a
 * valid destination binding) with the id to display; the view model asks URtsProxySubsystem for
 * the matching UCharacterProxy, then keeps itself in sync by listening to that proxy's single
 * OnChanged event, and every FieldNotify property below re-broadcasts automatically so bound
 *
 * The split of work between the two: the proxy reads through to the facades and never caches,
 * the view model caches exactly the flat values UMG can bind and pays for that with the
 * broadcasts. Anything the proxy can reach, this can display — including values from other
 * domains, which is why NationName sits next to CharacterName here.
 *
 * Every value is exposed FLAT: one property per struct member, never the struct itself, because
 * UMG can only bind a widget property to a single value. FCharacterDisplayInfo, FAbilities,
 * FCharacterVitals, FCharacterSkills and FSchedule are therefore taken apart here — the proxy
 * still hands them over whole, the view model publishes the individual fields. Ids follow the
 * same rule: the typed FCharacterId/FProvinceId stays (pass it to another Initialize) and its
 * int32 gets a ...Value twin for anything that has to display it.
 *
 * This is the pattern the other five view models follow: one class per domain, Initialize(Id),
 * flat FieldNotify properties fed from the proxy, and one HandleProxyChanged bound to OnChanged.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSPROXY_API UCharacterViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** The schedule entry for one hour of the day, 0-23. Out-of-range hours return SET_NONE. */
	UFUNCTION(BlueprintPure, Category="Character Viewmodel|Schedule")
	EScheduleEntryType GetScheduleAtHour(int32 InHour) const;

	/** The organization behind one of this character's positions, or an invalid id if out of range. */
	UFUNCTION(BlueprintPure, Category="Character Viewmodel|Positions")
	FOrganizationId GetPositionOrganizationId(int32 InIndex) const;

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False once the character's entity has been destroyed. Display keeps its last known values. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Character Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel", meta=(AllowPrivateAccess))
	FCharacterId CharacterId;

	/** CharacterId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel", meta=(AllowPrivateAccess))
	int32 CharacterIdValue = INDEX_NONE;

	//~ FCharacterDisplayInfo --------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Display", meta=(AllowPrivateAccess))
	FText CharacterName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Display", meta=(AllowPrivateAccess))
	FSlateBrush Portrait;

	//~ Location ---------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Location", meta=(AllowPrivateAccess))
	FProvinceId CurrentProvinceId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Location", meta=(AllowPrivateAccess))
	int32 CurrentProvinceIdValue = INDEX_NONE;

	/** Read through the province proxy — the character's own module never sees UProvinces. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Location", meta=(AllowPrivateAccess))
	FText CurrentProvinceName;

	/** The nation owning the province the character stands in. Two hops, resolved by the proxy. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Location", meta=(AllowPrivateAccess))
	FNationId CurrentNationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Location", meta=(AllowPrivateAccess))
	int32 CurrentNationIdValue = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Location", meta=(AllowPrivateAccess))
	FText CurrentNationName;

	//~ FAbilities -------------------------------------------------------------------------------
	// Widened from uint8 to int32 so they drop straight into the integer conversion nodes UMG has.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Abilities", meta=(AllowPrivateAccess))
	int32 AbilityStrength = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Abilities", meta=(AllowPrivateAccess))
	int32 AbilityDexterity = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Abilities", meta=(AllowPrivateAccess))
	int32 AbilityConstitution = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Abilities", meta=(AllowPrivateAccess))
	int32 AbilityIntelligence = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Abilities", meta=(AllowPrivateAccess))
	int32 AbilityWisdom = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Abilities", meta=(AllowPrivateAccess))
	int32 AbilityCharisma = 0;

	//~ FCharacterVitals -------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Vitals", meta=(AllowPrivateAccess))
	int32 Energy = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Vitals", meta=(AllowPrivateAccess))
	int32 Happiness = 0;

	//~ FCharacterSkills -------------------------------------------------------------------------
	// Prefixed because FAbilities and FCharacterSkills both carry a "Strength".

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Skills", meta=(AllowPrivateAccess))
	int32 SkillScience = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Skills", meta=(AllowPrivateAccess))
	int32 SkillStrength = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Skills", meta=(AllowPrivateAccess))
	int32 SkillMedicine = 0;

	//~ FSchedule --------------------------------------------------------------------------------
	// One property per hour so a 24-slot day view can bind each slot directly. GetScheduleAtHour
	// covers the loop-driven case.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	FScheduleId ScheduleId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	int32 ScheduleIdValue = INDEX_NONE;

	/** The library entry's name, read through the schedule proxy. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	FText ScheduleName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour00 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour01 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour02 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour03 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour04 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour05 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour06 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour07 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour08 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour09 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour10 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour11 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour12 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour13 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour14 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour15 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour16 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour17 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour18 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour19 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour20 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour21 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour22 = EScheduleEntryType::SET_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Schedule", meta=(AllowPrivateAccess))
	EScheduleEntryType ScheduleHour23 = EScheduleEntryType::SET_NONE;

	//~ Positions --------------------------------------------------------------------------------
	// FPosition is unwrapped into parallel arrays: an organization id and the role held there.

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Positions", meta=(AllowPrivateAccess))
	TArray<FOrganizationId> PositionOrganizationIds;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Positions", meta=(AllowPrivateAccess))
	TArray<int32> PositionOrganizationIdValues;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Positions", meta=(AllowPrivateAccess))
	TArray<FRoleId> PositionRoleIds;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Positions", meta=(AllowPrivateAccess))
	TArray<int32> PositionRoleIdValues;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Character Viewmodel|Positions", meta=(AllowPrivateAccess))
	int32 PositionCount = 0;

public:
	bool IsValid() const { return bIsValid; }
	FCharacterId GetCharacterId() const { return CharacterId; }
	int32 GetCharacterIdValue() const { return CharacterIdValue; }

	FText GetCharacterName() const { return CharacterName; }
	FSlateBrush GetPortrait() const { return Portrait; }

	FProvinceId GetCurrentProvinceId() const { return CurrentProvinceId; }
	int32 GetCurrentProvinceIdValue() const { return CurrentProvinceIdValue; }
	FText GetCurrentProvinceName() const { return CurrentProvinceName; }
	FNationId GetCurrentNationId() const { return CurrentNationId; }
	int32 GetCurrentNationIdValue() const { return CurrentNationIdValue; }
	FText GetCurrentNationName() const { return CurrentNationName; }

	int32 GetAbilityStrength() const { return AbilityStrength; }
	int32 GetAbilityDexterity() const { return AbilityDexterity; }
	int32 GetAbilityConstitution() const { return AbilityConstitution; }
	int32 GetAbilityIntelligence() const { return AbilityIntelligence; }
	int32 GetAbilityWisdom() const { return AbilityWisdom; }
	int32 GetAbilityCharisma() const { return AbilityCharisma; }

	int32 GetEnergy() const { return Energy; }
	int32 GetHappiness() const { return Happiness; }

	int32 GetSkillScience() const { return SkillScience; }
	int32 GetSkillStrength() const { return SkillStrength; }
	int32 GetSkillMedicine() const { return SkillMedicine; }

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

	const TArray<FOrganizationId>& GetPositionOrganizationIds() const { return PositionOrganizationIds; }
	const TArray<int32>& GetPositionOrganizationIdValues() const { return PositionOrganizationIdValues; }
	const TArray<FRoleId>& GetPositionRoleIds() const { return PositionRoleIds; }
	const TArray<int32>& GetPositionRoleIdValues() const { return PositionRoleIdValues; }
	int32 GetPositionCount() const { return PositionCount; }

protected:
	UFUNCTION(BlueprintCallable, Category="Character Viewmodel")
	void Initialize(FCharacterId InCharacterId);

	UFUNCTION(BlueprintNativeEvent, Category="Character Viewmodel")
	void Refresh();

	/** Fired once the proxy has been resolved and refers to a live entity, so subclasses don't have to poll IsValid. */
	UFUNCTION(BlueprintImplementableEvent, Category="Character Viewmodel")
	void OnProxyReady();

	/** Exposed for Blueprint subclasses that need to reach the proxy directly beyond the flat FieldNotify properties. */
	UPROPERTY(BlueprintReadOnly, Category="Character Viewmodel")
	TObjectPtr<UCharacterProxy> Proxy;

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void RefreshLocation();
	void RefreshSchedule();
	void RefreshPositions();
	void Unbind();

	UFUNCTION()
	void HandleProxyChanged();
};
