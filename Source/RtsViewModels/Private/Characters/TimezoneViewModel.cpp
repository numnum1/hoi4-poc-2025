#include "Characters/TimezoneViewModel.h"

#include "Characters.h"
#include "Provinces.h"
#include "RtsFacadeSubsystem.h"
#include "Time.h"

namespace
{
	/** The two speeds the clock's buttons switch between. */
	constexpr int32 NormalTimeSpeed = 1;
	constexpr int32 FastForwardTimeSpeed = 5;
}

void UTimezoneViewModel::Initialize(const FCharacterId InCharacterId)
{
	UCharacters* NewCharacters = nullptr;
	UProvinces* NewProvinces = nullptr;
	UTime* NewTime = nullptr;
	if (URtsFacadeSubsystem* Facades = URtsFacadeSubsystem::Get(this))
	{
		NewCharacters = Facades->GetCharactersContainer();
		NewProvinces = Facades->GetProvincesContainer();
		NewTime = Facades->GetTimeContainer();
	}

	if (Characters == NewCharacters && Time == NewTime && CharacterId == InCharacterId)
	{
		return;
	}

	Unbind();

	Characters = NewCharacters;
	Provinces = NewProvinces;
	Time = NewTime;

	CharacterId = InCharacterId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CharacterId);
	UE_MVVM_SET_PROPERTY_VALUE(CharacterIdValue, CharacterId.Value);

	if (Characters)
	{
		// Moving is the only character-side change that reaches this clock: a different province
		// means a different offset, with the same game time.
		Characters->OnCurrentProvinceChanged.AddDynamic(this, &UTimezoneViewModel::HandleCharacterEvent);
		Characters->OnCharacterDestroyed.AddDynamic(this, &UTimezoneViewModel::HandleCharacterEvent);
	}

	if (Time)
	{
		Time->OnCurrentTimeChanged.AddDynamic(this, &UTimezoneViewModel::HandleTimeEvent);
		Time->OnSpeedChanged.AddDynamic(this, &UTimezoneViewModel::HandleTimeEvent);
	}

	Refresh();
}

void UTimezoneViewModel::HandleCharacterEvent(const FCharacterId InCharacterId)
{
	if (InCharacterId == CharacterId)
	{
		Refresh();
	}
}

void UTimezoneViewModel::HandleTimeEvent()
{
	Refresh();
}

void UTimezoneViewModel::Refresh_Implementation()
{
	if (Characters == nullptr || Time == nullptr)
	{
		UE_MVVM_SET_PROPERTY_VALUE(bIsValid, false);
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Characters->IsAlive(CharacterId));

	const FProvinceId NewProvinceId = Characters->GetCurrentProvince(CharacterId);
	const bool bHasProvince = Provinces != nullptr && NewProvinceId.IsValid();

	UE_MVVM_SET_PROPERTY_VALUE(ProvinceId, NewProvinceId);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceIdValue, NewProvinceId.Value);
	UE_MVVM_SET_PROPERTY_VALUE(ProvinceName, bHasProvince ? Provinces->GetDisplayInfo(NewProvinceId).Name : FText::GetEmpty());

	const int32 NewOffset = bHasProvince ? Provinces->GetTimezoneOffset(NewProvinceId) : 0;
	UE_MVVM_SET_PROPERTY_VALUE(TimezoneOffset, NewOffset);
	UE_MVVM_SET_PROPERTY_VALUE(TimezoneOffsetText,
		FText::FromString(FString::Printf(TEXT("UTC%+d"), NewOffset)));

	// Same shift the hourly sim applies to a character's schedule, so both agree on what hour it is
	// where the character stands.
	const FDateTime LocalTime = Time->GetCurrentTime() + FTimespan::FromHours(NewOffset);

	UE_MVVM_SET_PROPERTY_VALUE(LocalHour, LocalTime.GetHour());
	UE_MVVM_SET_PROPERTY_VALUE(LocalMinute, LocalTime.GetMinute());
	UE_MVVM_SET_PROPERTY_VALUE(LocalTimeText,
		FText::FromString(FString::Printf(TEXT("%02d:%02d"), LocalTime.GetHour(), LocalTime.GetMinute())));

	UE_MVVM_SET_PROPERTY_VALUE(LocalDay, LocalTime.GetDay());
	UE_MVVM_SET_PROPERTY_VALUE(LocalMonth, LocalTime.GetMonth());
	UE_MVVM_SET_PROPERTY_VALUE(LocalYear, LocalTime.GetYear());
	UE_MVVM_SET_PROPERTY_VALUE(LocalDateText,
		FText::FromString(FString::Printf(TEXT("%04d-%02d-%02d %02d:%02d"),
			LocalTime.GetYear(), LocalTime.GetMonth(), LocalTime.GetDay(),
			LocalTime.GetHour(), LocalTime.GetMinute())));

	const int32 NewSpeed = Time->GetSpeed();
	UE_MVVM_SET_PROPERTY_VALUE(Speed, NewSpeed);
	UE_MVVM_SET_PROPERTY_VALUE(bIsFastForward, NewSpeed > NormalTimeSpeed);
}

void UTimezoneViewModel::CallOnFastForwardClicked()
{
	SetSpeed(FastForwardTimeSpeed);
	OnFastForwardClicked.Broadcast();
}

void UTimezoneViewModel::CallOnNormalSpeedClicked()
{
	SetSpeed(NormalTimeSpeed);
	OnNormalSpeedClicked.Broadcast();
}

void UTimezoneViewModel::SetSpeed(const int32 InSpeed)
{
	if (Time == nullptr)
	{
		return;
	}

	// SetSpeed broadcasts OnSpeedChanged, which this view model is bound to, so the displayed Speed
	// is refreshed by the clock saying so — never written here from what was just requested.
	Time->SetSpeed(InSpeed);
}

void UTimezoneViewModel::Unbind()
{
	if (Characters)
	{
		Characters->OnCurrentProvinceChanged.RemoveAll(this);
		Characters->OnCharacterDestroyed.RemoveAll(this);
	}

	if (Time)
	{
		Time->OnCurrentTimeChanged.RemoveAll(this);
		Time->OnSpeedChanged.RemoveAll(this);
	}
}

void UTimezoneViewModel::BeginDestroy()
{
	Unbind();
	Super::BeginDestroy();
}
