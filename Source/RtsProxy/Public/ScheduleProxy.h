#pragma once

#include "CoreMinimal.h"
#include "RtsEntityProxy.h"
#include "RtsIds.h"
#include "Schedule.h"
#include "ScheduleProxy.generated.h"

class USchedules;

/**
 * One authored schedule from USchedules' library.
 *
 * Unlike the other proxies this one does not stand for an entity: the library is filled once at
 * scenario load and read-only afterwards, so IsAlive only reports whether the id names an entry
 * and there are no events to subscribe to. The schedule a character actually runs is its own
 * copy — reach that through UCharacterProxy::GetSchedule, and this one only for its name.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UScheduleProxy : public URtsEntityProxy
{
	GENERATED_BODY()

public:
	/** Called by URtsProxySubsystem right after construction. Not for general use. */
	void Setup(URtsProxySubsystem* InProxies, FScheduleId InScheduleId, USchedules* InSchedules);

	UFUNCTION(BlueprintPure, Category="Schedule")
	FScheduleId GetId() const { return ScheduleId; }

	/** True while the id names an entry in the library. Never becomes false during play. */
	virtual bool IsAlive() const override;

	UFUNCTION(BlueprintPure, Category="Schedule")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category="Schedule")
	FSchedule GetSchedule() const;

	/** The entry for one hour of the day, 0-23. Out-of-range hours return SET_NONE. */
	UFUNCTION(BlueprintPure, Category="Schedule")
	EScheduleEntryType GetScheduleAtHour(int32 InHour) const;

	/** The facade underneath, for the few calls no proxy covers. */
	USchedules* GetContainer() const { return Schedules; }

private:
	FScheduleId ScheduleId;

	UPROPERTY()
	TObjectPtr<USchedules> Schedules;
};
