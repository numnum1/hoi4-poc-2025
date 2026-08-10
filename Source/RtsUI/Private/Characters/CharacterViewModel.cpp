#include "Characters/CharacterViewModel.h"

#include "Characters.h"
#include "Characters/PositionViewModel.h"
#include "NationInfo.h"
#include "Nations.h"
#include "Organizations.h"
#include "ProvinceInfo.h"
#include "Provinces.h"
#include "Roles.h"
#include "RtsGameStateComponent.h"
#include "Schedules.h"

void UCharacterViewModel::Initialize(const FCharacterId InCharacterId)
{
	UCharacters* NewCharacters = FindRtsGameStateComponent<UCharacters>(this);
	UProvinces* NewProvinces = FindRtsGameStateComponent<UProvinces>(this);
	UNations* NewNations = FindRtsGameStateComponent<UNations>(this);
	USchedules* NewSchedules = FindRtsGameStateComponent<USchedules>(this);
	UOrganizations* NewOrganizations = FindRtsGameStateComponent<UOrganizations>(this);
	URoles* NewRoles = FindRtsGameStateComponent<URoles>(this);

	if (Characters == NewCharacters && CharacterId == InCharacterId)
	{
		return;
	}

	Unbind();

	Characters = NewCharacters;
	Provinces = NewProvinces;
	Nations = NewNations;
	Schedules = NewSchedules;
	Organizations = NewOrganizations;
	Roles = NewRoles;

	CharacterId = InCharacterId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterIdValue, CharacterId.Value);

	if (Characters)
	{
		// Every event that can change something this class displays, each filtered to this id in
		// the single HandleCharacterEvent handler below.
		Characters->OnCurrentProvinceChanged.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		Characters->OnScheduleChanged.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		Characters->OnPositionChanged.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		Characters->OnSimulated.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		Characters->OnCharacterDestroyed.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
	}

	Refresh();

	if (Characters && bIsValid)
	{
		OnCharacterReady();
	}
}

void UCharacterViewModel::HandleCharacterEvent(const FCharacterId InCharacterId)
{
	if (InCharacterId == CharacterId)
	{
		Refresh();
	}
}

void UCharacterViewModel::Refresh_Implementation()
{
	if (Characters == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Characters->IsAlive(CharacterId));

	const FCharacterDisplayInfo DisplayInfo = Characters->GetDisplayInfo(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterName, DisplayInfo.CharacterName);
	UE_MVVM_SET_PROPERTY_VALUE(Portrait, DisplayInfo.Portrait);

	const FAbilities NewAbilities = Characters->GetAbilities(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(AbilityStrength, static_cast<int32>(NewAbilities.Strength));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityDexterity, static_cast<int32>(NewAbilities.Dexterity));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityConstitution, static_cast<int32>(NewAbilities.Constitution));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityIntelligence, static_cast<int32>(NewAbilities.Intelligence));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityWisdom, static_cast<int32>(NewAbilities.Wisdom));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityCharisma, static_cast<int32>(NewAbilities.Charisma));

	const FCharacterVitals NewVitals = Characters->GetVitals(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(Energy, NewVitals.Energy);
	UE_MVVM_SET_PROPERTY_VALUE(Happiness, NewVitals.Happiness);

	const FCharacterSkills NewSkills = Characters->GetSkills(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(SkillScience, NewSkills.Science);
	UE_MVVM_SET_PROPERTY_VALUE(SkillStrength, NewSkills.Strength);
	UE_MVVM_SET_PROPERTY_VALUE(SkillMedicine, NewSkills.Medicine);

	RefreshLocation();
	RefreshSchedule();
	RefreshPositions();
}

void UCharacterViewModel::RefreshLocation()
{
	// Province and nation are read through their own facades — the whole reason the view models
	// live here rather than in Characters, which is not allowed to know either of them.
	const FProvinceId NewProvinceId = Characters->GetCurrentProvince(CharacterId);
	const bool bHasProvince = Provinces != nullptr && NewProvinceId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceId, NewProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceIdValue, NewProvinceId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceName, bHasProvince ? Provinces->GetDisplayInfo(NewProvinceId).Name : FText::GetEmpty());

	const FNationId NewNationId = bHasProvince ? Provinces->GetOwningNation(NewProvinceId) : FNationId();
	const bool bHasNation = Nations != nullptr && NewNationId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(CurrentNationId, NewNationId);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentNationIdValue, NewNationId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentNationName, bHasNation ? Nations->GetNationInfo(NewNationId).NationName : FText::GetEmpty());
}

void UCharacterViewModel::RefreshSchedule()
{
	const FScheduleId TemplateId = Characters->GetScheduleId(CharacterId);
	const bool bHasTemplate = Schedules != nullptr && TemplateId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(ScheduleId, TemplateId);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleIdValue, TemplateId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleName, bHasTemplate ? Schedules->GetScheduleName(TemplateId) : FText::GetEmpty());

	const FSchedule NewSchedule = Characters->GetSchedule(CharacterId);
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
	const TArray<FPosition> CharacterPositions = Characters->GetPositions(CharacterId);

	TArray<TObjectPtr<UPositionViewModel>> NewPositions;
	NewPositions.Reserve(CharacterPositions.Num());

	for (int32 Index = 0; Index < CharacterPositions.Num(); ++Index)
	{
		if (!PositionEntryCache.IsValidIndex(Index))
		{
			PositionEntryCache.Add(NewObject<UPositionViewModel>(this));
		}

		UPositionViewModel* Entry = PositionEntryCache[Index];
		Entry->SetSource(CharacterPositions[Index], CharacterId, Organizations, Roles);
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
	if (Characters)
	{
		Characters->OnCurrentProvinceChanged.RemoveAll(this);
		Characters->OnScheduleChanged.RemoveAll(this);
		Characters->OnPositionChanged.RemoveAll(this);
		Characters->OnSimulated.RemoveAll(this);
		Characters->OnCharacterDestroyed.RemoveAll(this);
	}
}

void UCharacterViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
