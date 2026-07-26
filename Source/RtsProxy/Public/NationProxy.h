#pragma once

#include "CoreMinimal.h"
#include "RtsEntityProxy.h"
#include "RtsIds.h"
#include "NationProxy.generated.h"

class UCharacterProxy;
class UNations;
class UOrganizationProxy;
class UProvinceProxy;

/**
 * One nation, as an object you can navigate from instead of an id you have to carry around.
 *
 * Everything is read through UNations on each call. Get one from URtsProxySubsystem::GetNation,
 * or from any other proxy that leads here. See URtsEntityProxy for the rules all proxies follow.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UNationProxy : public URtsEntityProxy
{
	GENERATED_BODY()

public:
	/** Called by URtsProxySubsystem right after construction. Not for general use. */
	void Setup(URtsProxySubsystem* InProxies, FNationId InNationId, UNations* InNations);

	UFUNCTION(BlueprintPure, Category="Nation")
	FNationId GetId() const { return NationId; }

	virtual bool IsAlive() const override;

	//~ Display ----------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Nation")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category="Nation")
	FSlateBrush GetFlag() const;

	//~ Navigation -------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Nation")
	TArray<UProvinceProxy*> GetProvinces() const;

	UFUNCTION(BlueprintPure, Category="Nation")
	int32 GetProvinceCount() const;

	/** Everyone standing in any province this nation owns. */
	UFUNCTION(BlueprintPure, Category="Nation")
	TArray<UCharacterProxy*> GetCharacters() const;

	UFUNCTION(BlueprintPure, Category="Nation")
	TArray<UOrganizationProxy*> GetSettledOrganizations() const;

	//~ Mutations --------------------------------------------------------------------------------

	/** Destroys the entity. The name and flag stay behind, and so does this proxy. */
	UFUNCTION(BlueprintCallable, Category="Nation")
	void DestroyNation();

	/** The facade underneath, for the few calls no proxy covers. */
	UNations* GetContainer() const { return Nations; }

protected:
	virtual void BindEvents() override;
	virtual void UnbindEvents() override;

private:
	UFUNCTION()
	void HandleNationEvent(FNationId InNationId);

	FNationId NationId;

	UPROPERTY()
	TObjectPtr<UNations> Nations;
};
