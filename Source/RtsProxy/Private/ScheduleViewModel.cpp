#include "ScheduleViewModel.h"

#include "RtsProxySubsystem.h"
#include "ScheduleProxy.h"

void UScheduleViewModel::Initialize(const FScheduleId InScheduleId)
{
	UScheduleProxy* NewProxy = nullptr;
	if (URtsProxySubsystem* ProxySubsystem = URtsProxySubsystem::Get(this))
	{
		NewProxy = ProxySubsystem->GetSchedule(InScheduleId);
	}

	if (Proxy == NewProxy && ScheduleId == InScheduleId)
	{
		return;
	}

	Proxy = NewProxy;

	ScheduleId = InScheduleId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ScheduleId);
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleIdValue, ScheduleId.Value);

	Refresh();
}

void UScheduleViewModel::Refresh()
{
	if (Proxy == nullptr)
	{
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bIsValid, Proxy->IsAlive());
	UE_MVVM_SET_PROPERTY_VALUE(ScheduleName, Proxy->GetDisplayName());

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
