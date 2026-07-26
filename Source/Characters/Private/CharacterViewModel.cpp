#include "CharacterViewModel.h"

#include "Characters.h"

void UCharacterViewModel::Initialize(UCharacters* InCharacters, const FCharacterId InCharacterId)
{
	if (CharactersContainer == InCharacters && CharacterId == InCharacterId)
	{
		return;
	}

	Unbind();

	CharactersContainer = InCharacters;

	CharacterId = InCharacterId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterIdValue, CharacterId.Value);

	if (CharactersContainer)
	{
		CharactersContainer->OnCurrentProvinceChanged.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		CharactersContainer->OnScheduleChanged.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		CharactersContainer->OnPositionChanged.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		CharactersContainer->OnSimulated.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
		CharactersContainer->OnCharacterDestroyed.AddDynamic(this, &UCharacterViewModel::HandleCharacterEvent);
	}

	Refresh();
}

void UCharacterViewModel::HandleCharacterEvent(const FCharacterId InCharacterId)
{
	if (InCharacterId == CharacterId)
	{
		Refresh();
	}
}

void UCharacterViewModel::Refresh()
{
	if (CharactersContainer == nullptr || !CharacterId.IsValid())
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, CharactersContainer->IsAlive(CharacterId));

	const FCharacterDisplayInfo DisplayInfo = CharactersContainer->GetDisplayInfo(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterName, DisplayInfo.CharacterName);
	UE_MVVM_SET_PROPERTY_VALUE(Portrait, DisplayInfo.Portrait);

	const FProvinceId NewProvinceId = CharactersContainer->GetCurrentProvince(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceId, NewProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentProvinceIdValue, NewProvinceId.Value);

	const FAbilities NewAbilities = CharactersContainer->GetAbilities(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(AbilityStrength, static_cast<int32>(NewAbilities.Strength));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityDexterity, static_cast<int32>(NewAbilities.Dexterity));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityConstitution, static_cast<int32>(NewAbilities.Constitution));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityIntelligence, static_cast<int32>(NewAbilities.Intelligence));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityWisdom, static_cast<int32>(NewAbilities.Wisdom));
	UE_MVVM_SET_PROPERTY_VALUE(AbilityCharisma, static_cast<int32>(NewAbilities.Charisma));

	const FCharacterVitals NewVitals = CharactersContainer->GetVitals(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(Energy, NewVitals.Energy);
	UE_MVVM_SET_PROPERTY_VALUE(Happiness, NewVitals.Happiness);

	const FCharacterSkills NewSkills = CharactersContainer->GetSkills(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(SkillScience, NewSkills.Science);
	UE_MVVM_SET_PROPERTY_VALUE(SkillStrength, NewSkills.Strength);
	UE_MVVM_SET_PROPERTY_VALUE(SkillMedicine, NewSkills.Medicine);

	RefreshSchedule();
	RefreshPositions();
}

void UCharacterViewModel::RefreshSchedule()
{
	const FScheduleId NewScheduleId = CharactersContainer->GetScheduleId(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleId, NewScheduleId);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleIdValue, NewScheduleId.Value);

	const FSchedule NewSchedule = CharactersContainer->GetSchedule(CharacterId);
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
	const TArray<FPosition> Positions = CharactersContainer->GetPositions(CharacterId);

	TArray<FOrganizationId> NewOrganizationIds;
	TArray<int32> NewOrganizationIdValues;
	NewOrganizationIds.Reserve(Positions.Num());
	NewOrganizationIdValues.Reserve(Positions.Num());
	for (const FPosition& Position : Positions)
	{
		NewOrganizationIds.Add(Position.OrganizationId);
		NewOrganizationIdValues.Add(Position.OrganizationId.Value);
	}

	UE_MVVM_SET_PROPERTY_VALUE(PositionOrganizationIds, NewOrganizationIds);
	UE_MVVM_SET_PROPERTY_VALUE(PositionOrganizationIdValues, NewOrganizationIdValues);
	UE_MVVM_SET_PROPERTY_VALUE(PositionCount, Positions.Num());
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

FOrganizationId UCharacterViewModel::GetPositionOrganizationId(const int32 InIndex) const
{
	return PositionOrganizationIds.IsValidIndex(InIndex) ? PositionOrganizationIds[InIndex] : FOrganizationId();
}

void UCharacterViewModel::Unbind()
{
	if (CharactersContainer)
	{
		CharactersContainer->OnCurrentProvinceChanged.RemoveAll(this);
		CharactersContainer->OnScheduleChanged.RemoveAll(this);
		CharactersContainer->OnPositionChanged.RemoveAll(this);
		CharactersContainer->OnSimulated.RemoveAll(this);
		CharactersContainer->OnCharacterDestroyed.RemoveAll(this);
	}
}

void UCharacterViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
