#include "ScheduleProxy.h"

#include "Schedules.h"

void UScheduleProxy::Setup(URtsProxySubsystem* InProxies, const FScheduleId InScheduleId, USchedules* InSchedules)
{
	Proxies = InProxies;
	ScheduleId = InScheduleId;
	IdValue = InScheduleId.Value;
	Schedules = InSchedules;
	// No BindEvents: the library never changes after scenario load.
}

bool UScheduleProxy::IsAlive() const
{
	return Schedules != nullptr && Schedules->IsValidSchedule(ScheduleId);
}

FText UScheduleProxy::GetDisplayName() const
{
	return Schedules ? Schedules->GetScheduleName(ScheduleId) : FText::GetEmpty();
}

FSchedule UScheduleProxy::GetSchedule() const
{
	return Schedules ? Schedules->GetSchedule(ScheduleId) : FSchedule();
}

EScheduleEntryType UScheduleProxy::GetScheduleAtHour(const int32 InHour) const
{
	if (InHour < 0 || InHour > 23)
	{
		return EScheduleEntryType::SET_NONE;
	}
	return GetSchedule().GetAtHour(static_cast<uint8>(InHour));
}
