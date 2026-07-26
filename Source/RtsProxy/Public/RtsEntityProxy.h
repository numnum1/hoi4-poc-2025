#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RtsEntityProxy.generated.h"

class URtsProxySubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRtsProxyChanged);

/**
 * Base class for the per-entity proxies handed out by URtsProxySubsystem.
 *
 * A proxy is a convenience object, not a second copy of the game state: it holds nothing but its
 * own id and a pointer to the facade it reads through, and every getter goes straight to that
 * facade. Nothing here can go stale, because nothing here is cached. That is the whole contract,
 * and it is what separates a proxy from a view model — the view models above them do cache, and
 * pay for it with FieldNotify broadcasts.
 *
 * The one thing a proxy does own is OnChanged: each subclass subscribes to whichever facade
 * events concern its own id and collapses them into this single delegate, so callers bind once
 * instead of to five separate events with an id comparison in each handler.
 *
 * Proxies are cached one per id and live as long as the world does, so it is safe to hold one in
 * a variable. A proxy for a destroyed entity stays usable and reports IsAlive() == false rather
 * than becoming dangling — ids are never reused, so the proxy can never come to mean something
 * else. Deliberately not built for bulk work: this is the player-facing reading layer, the
 * simulation goes through Mass.
 */
UCLASS(Abstract, BlueprintType)
class RTSPROXY_API URtsEntityProxy : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Fires when anything reachable through this proxy changed, including its entity being
	 * destroyed. Carries no payload — ask the proxy for whatever you actually display.
	 */
	UPROPERTY(BlueprintAssignable, Category="Proxy")
	FRtsProxyChanged OnChanged;

	/** False once the entity is gone. The proxy stays valid and keeps answering. */
	UFUNCTION(BlueprintPure, Category="Proxy")
	virtual bool IsAlive() const { return false; }

	/** The id as a plain int32, for logging and for widgets that cannot take the typed id. */
	UFUNCTION(BlueprintPure, Category="Proxy")
	int32 GetIdValue() const { return IdValue; }

	/** The registry that owns this proxy — used to reach proxies of other domains. */
	UFUNCTION(BlueprintPure, Category="Proxy")
	URtsProxySubsystem* GetProxies() const { return Proxies; }

protected:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	/** Subscribe to the facade events that concern this id. Called once, after construction. */
	virtual void BindEvents() {}

	/** Undo BindEvents. Called from BeginDestroy and when the subsystem shuts down. */
	virtual void UnbindEvents() {}

	void NotifyChanged() { OnChanged.Broadcast(); }

	UPROPERTY()
	TObjectPtr<URtsProxySubsystem> Proxies;

	int32 IdValue = INDEX_NONE;

	friend class URtsProxySubsystem;
};
