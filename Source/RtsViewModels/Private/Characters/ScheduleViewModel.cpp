#include "Characters/ScheduleViewModel.h"

#include "Characters.h"
#include "Provinces.h"
#include "RtsFacadeSubsystem.h"
#include "Schedules.h"

void UScheduleViewModel::Initialize(const FCharacterId InCharacterId)
{
	UCharacters* NewCharacters = nullptr;
	USchedules* NewSchedules = nullptr;
	UProvinces* NewProvinces = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewCharacters = Facades->GetCharactersContainer();
		NewSchedules = Facades->GetSchedulesContainer();
		NewProvinces = Facades->GetProvincesContainer();
	}

	if (Characters == NewCharacters && CharacterId == InCharacterId)
	{
		return;
	}

	Unbind();

	Characters = NewCharacters;
	Schedules = NewSchedules;
	Provinces = NewProvinces;

	CharacterId = InCharacterId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterIdValue, CharacterId.Value);

	if (Characters)
	{
		Characters->OnCurrentProvinceChanged.AddDynamic(this, &UScheduleViewModel::HandleCharacterEvent);
		Characters->OnScheduleChanged.AddDynamic(this, &UScheduleViewModel::HandleCharacterEvent);
		Characters->OnPositionChanged.AddDynamic(this, &UScheduleViewModel::HandleCharacterEvent);
		Characters->OnSimulated.AddDynamic(this, &UScheduleViewModel::HandleCharacterEvent);
		Characters->OnCharacterDestroyed.AddDynamic(this, &UScheduleViewModel::HandleCharacterEvent);
	}

	Refresh();
}

void UScheduleViewModel::HandleCharacterEvent(const FCharacterId InCharacterId)
{
	if (InCharacterId == CharacterId)
	{
		Refresh();
	}
}

void UScheduleViewModel::Refresh_Implementation()
{
	if (Characters == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Characters->IsAlive(CharacterId));

	const FScheduleId TemplateId = Characters->GetScheduleId(CharacterId);
	const bool bHasTemplate = Schedules != nullptr && TemplateId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(ScheduleId, TemplateId);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleIdValue, TemplateId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleName, bHasTemplate ? Schedules->GetScheduleName(TemplateId) : FText::GetEmpty());

	const FProvinceId ProvinceId = Characters->GetCurrentProvince(CharacterId);
	const bool bHasProvince = Provinces != nullptr && ProvinceId.IsValid();
	UE_MVVM_SET_PROPERTY_VALUE(TimezoneOffset, bHasProvince ? Provinces->GetTimezoneOffset(ProvinceId) : 0);

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

EScheduleEntryType UScheduleViewModel::GetScheduleAtHour(const int32 InHour) const
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

void UScheduleViewModel::Unbind()
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

void UScheduleViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
