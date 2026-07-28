#include "CharacterViewModel.h"

#include "CharacterProxy.h"
#include "NationProxy.h"
#include "OrganizationProxy.h"
#include "PositionViewModel.h"
#include "ProvinceProxy.h"
#include "RtsProxySubsystem.h"
#include "ScheduleProxy.h"

void UCharacterViewModel::Initialize(const FCharacterId InCharacterId)
{
	UCharacterProxy* NewProxy = nullptr;
	if (URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this))
	{
		NewProxy = ProxySubsystem->GetCharacter(InCharacterId);
	}

	if (Proxy == NewProxy && CharacterId == InCharacterId)
	{
		return;
	}

	Unbind();

	Proxy = NewProxy;

	CharacterId = InCharacterId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterIdValue, CharacterId.Value);

	if (Proxy)
	{
		// One subscription instead of five: the proxy already folded the facade's events into
		// OnChanged and filtered them by this character's id.
		Proxy->OnChanged.AddDynamic(this, &UCharacterViewModel::HandleProxyChanged);
	}

	Refresh();

	if (Proxy && bIsValid)
	{
		OnProxyReady();
	}
}

void UCharacterViewModel::HandleProxyChanged()
{
	Refresh();
}

void UCharacterViewModel::Refresh_Implementation()
{
	if (Proxy == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Proxy->IsAlive());

	UE_MVVM_SET_PROPERTY_VALUE(CharacterName, Proxy->GetDisplayName());
	UE_MVVM_SET_PROPERTY_VALUE(Portrait, Proxy->GetPortrait());

	const FAbilities NewAbilities = Proxy->GetAbilities();
	UE_MVVM_SET_PROPERTY_VALUE(AbilityStrength, static_cast<int32>(NewAbilities.Strength));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityDexterity, static_cast<int32>(NewAbilities.Dexterity));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityConstitution, static_cast<int32>(NewAbilities.Constitution));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityIntelligence, static_cast<int32>(NewAbilities.Intelligence));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityWisdom, static_cast<int32>(NewAbilities.Wisdom));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityCharisma, static_cast<int32>(NewAbilities.Charisma));

	const FCharacterVitals NewVitals = Proxy->GetVitals();
	UE_MVVM_SET_PROPERTY_VALUE(Energy, NewVitals.Energy);
	UE_MVVM_SET_PROPERTY_VALUE(Happiness, NewVitals.Happiness);

	const FCharacterSkills NewSkills = Proxy->GetSkills();
	UE_MVVM_SET_PROPERTY_VALUE(SkillScience, NewSkills.Science);
	UE_MVVM_SET_PROPERTY_VALUE(SkillStrength, NewSkills.Strength);
	UE_MVVM_SET_PROPERTY_VALUE(SkillMedicine, NewSkills.Medicine);

	RefreshLocation();
	RefreshSchedule();
	RefreshPositions();
}

void UCharacterViewModel::RefreshLocation()
{
	// Province and nation come from proxies of other modules — the whole reason the view models
	// live here rather than in Characters, which is not allowed to know either of them.
	const UProvinceProxy* Province = Proxy->GetProvince();
	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceId, Province ? Province->GetId() : FProvinceId());
	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceIdValue, Province ? Province->GetId().Value : INDEX_NONE);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceName, Province ? Province->GetDisplayName() : FText::GetEmpty());

	const UNationProxy* Nation = Province ? Province->GetOwner() : nullptr;
	UE_MVVM_SET_PROPERTY_VALUE(CurrentNationId, Nation ? Nation->GetId() : FNationId());
	UE_MVVM_SET_PROPERTY_VALUE(CurrentNationIdValue, Nation ? Nation->GetId().Value : INDEX_NONE);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentNationName, Nation ? Nation->GetDisplayName() : FText::GetEmpty());
}

void UCharacterViewModel::RefreshSchedule()
{
	const UScheduleProxy* Template = Proxy->GetScheduleTemplate();
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleId, Template ? Template->GetId() : FScheduleId());
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleIdValue, Template ? Template->GetId().Value : INDEX_NONE);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleName, Template ? Template->GetDisplayName() : FText::GetEmpty());

	const FSchedule NewSchedule = Proxy->GetSchedule();
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour00, NewSchedule.Zero);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour01, NewSchedule.One);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour02, NewSchedule.Two);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour03, NewSchedule.Three);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour04, NewSchedule.Four);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour05, NewSchedule.Five);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour06, NewSchedule.Six);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour07, NewSchedule.Seven);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour08, NewSchedule.Eight);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour09, NewSchedule.Nine);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour10, NewSchedule.Ten);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour11, NewSchedule.Eleven);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour12, NewSchedule.Twelve);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour13, NewSchedule.Thirteen);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour14, NewSchedule.Fourteen);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour15, NewSchedule.Fifteen);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour16, NewSchedule.Sixteen);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour17, NewSchedule.Seventeen);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour18, NewSchedule.Eighteen);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour19, NewSchedule.Nineteen);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour20, NewSchedule.Twenty);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour21, NewSchedule.TwentyOne);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour22, NewSchedule.TwentyTwo);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleHour23, NewSchedule.TwentyThree);
}

void UCharacterViewModel::RefreshPositions()
{
	const int32 Count = Proxy->GetOrganizationCount();

	TArray<TObjectPtr<UPositionViewModel>> NewPositions;
	NewPositions.Reserve(Count);

	for (int32 Index = 0; Index < Count; ++Index)
	{
		if (!PositionEntryCache.IsValidIndex(Index))
		{
			PositionEntryCache.Add(NewObject<UPositionViewModel>(this));
		}

		UPositionViewModel* Entry = PositionEntryCache[Index];
		Entry->SetSource(Proxy, Index);
		NewPositions.Add(Entry);
	}

	UE_MVVM_SET_PROPERTY_VALUE(Positions, NewPositions);
	UE_MVVM_SET_PROPERTY_VALUE(PositionCount, NewPositions.Num());
}

EScheduleEntryType UCharacterViewModel::GetScheduleAtHour(const int32 InHour) const
{
	switch (InHour)
	{
	case 0:  return ScheduleHour00;
	case 1:  return ScheduleHour01;
	case 2:  return ScheduleHour02;
	case 3:  return ScheduleHour03;
	case 4:  return ScheduleHour04;
	case 5:  return ScheduleHour05;
	case 6:  return ScheduleHour06;
	case 7:  return ScheduleHour07;
	case 8:  return ScheduleHour08;
	case 9:  return ScheduleHour09;
	case 10: return ScheduleHour10;
	case 11: return ScheduleHour11;
	case 12: return ScheduleHour12;
	case 13: return ScheduleHour13;
	case 14: return ScheduleHour14;
	case 15: return ScheduleHour15;
	case 16: return ScheduleHour16;
	case 17: return ScheduleHour17;
	case 18: return ScheduleHour18;
	case 19: return ScheduleHour19;
	case 20: return ScheduleHour20;
	case 21: return ScheduleHour21;
	case 22: return ScheduleHour22;
	case 23: return ScheduleHour23;
	default: return EScheduleEntryType::SET_NONE;
	}
}

void UCharacterViewModel::Unbind()
{
	if (Proxy)
	{
		Proxy->OnChanged.RemoveAll(this);
	}
}

void UCharacterViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
