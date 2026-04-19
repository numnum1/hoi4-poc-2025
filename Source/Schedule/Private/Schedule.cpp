#include "Schedule.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "FScheduleModule"

void FScheduleModule::StartupModule()
{
}

void FScheduleModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FScheduleModule, Schedule)

USchedule::USchedule()
{
	SetIsReplicatedByDefault(true);

	Entries = {
		FScheduleEntry{
			FText::FromString("Sleep"),
			FLinearColor::Blue,
			{
				FInt32Range{
						0,8
				},
				FInt32Range{
					22,24
				}
			}
		}
	};
}

void USchedule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USchedule, Entries);
}

bool USchedule::FindEntry(FScheduleEntry& OutEntry, const int32 InHour) const
{
	for (const auto& Entry : Entries)
	{
		for (const auto& Time : Entry.Times)
		{
			if (Time.Contains(InHour))
			{
				OutEntry = Entry;
				return true;
			}
		}
	}
	return false;
}
