#pragma once

#include "CoreMinimal.h"
#include "Abilities.h"
#include "CharacterInfo.h"
#include "RtsEntityProxy.h"
#include "RtsIds.h"
#include "Schedule.h"
#include "CharacterProxy.generated.h"

class UBuildingProxy;
class UCharacters;
class UNationProxy;
class UOrganizationProxy;
class UProvinceProxy;
class UScheduleProxy;

/**
 * One character, as an object you can navigate from instead of an id you have to carry around.
 *
 * Everything is read through UCharacters on each call, so a proxy never disagrees with the
 * simulation. Get one from URtsProxySubsystem::GetCharacter, or from any other proxy that leads
 * here. See URtsEntityProxy for the rules all proxies follow.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UCharacterProxy : public URtsEntityProxy
{
	GENERATED_BODY()

public:
	/** Called by URtsProxySubsystem right after construction. Not for general use. */
	void Setup(URtsProxySubsystem* InProxies, FCharacterId InCharacterId, UCharacters* InCharacters);

	UFUNCTION(BlueprintPure, Category="Character")
	FCharacterId GetId() const { return CharacterId; }

	virtual bool IsAlive() const override;

	//~ Display ----------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Character")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category="Character")
	FSlateBrush GetPortrait() const;

	//~ Stats ------------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Character")
	FAbilities GetAbilities() const;

	UFUNCTION(BlueprintPure, Category="Character")
	FCharacterVitals GetVitals() const;

	UFUNCTION(BlueprintPure, Category="Character")
	FCharacterSkills GetSkills() const;

	//~ Schedule ---------------------------------------------------------------------------------

	/** The schedule this character actually runs — its own copy, not the library entry. */
	UFUNCTION(BlueprintPure, Category="Character|Schedule")
	FSchedule GetSchedule() const;

	/** The entry for one hour of the day, 0-23. Out-of-range hours return SET_NONE. */
	UFUNCTION(BlueprintPure, Category="Character|Schedule")
	EScheduleEntryType GetScheduleAtHour(int32 InHour) const;

	/** The USchedules library entry this character's schedule was copied from, for its name. */
	UFUNCTION(BlueprintPure, Category="Character|Schedule")
	UScheduleProxy* GetScheduleTemplate() const;

	//~ Navigation -------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Character")
	UProvinceProxy* GetProvince() const;

	/** The nation owning the province this character is in — the two hops in one call. */
	UFUNCTION(BlueprintPure, Category="Character")
	UNationProxy* GetNation() const;

	UFUNCTION(BlueprintPure, Category="Character|Organizations")
	TArray<UOrganizationProxy*> GetOrganizations() const;

	/** The organization behind one of this character's positions, or nullptr if out of range. */
	UFUNCTION(BlueprintPure, Category="Character|Organizations")
	UOrganizationProxy* GetOrganization(int32 InIndex) const;

	UFUNCTION(BlueprintPure, Category="Character|Organizations")
	int32 GetOrganizationCount() const;

	UFUNCTION(BlueprintPure, Category="Character|Organizations")
	bool IsMemberOf(const UOrganizationProxy* InOrganization) const;

	/** Every building owned by this character. Walks all buildings — fine for a UI panel. */
	UFUNCTION(BlueprintPure, Category="Character")
	TArray<UBuildingProxy*> GetOwnedBuildings() const;

	//~ Actions ----------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category="Character")
	void MoveTo(const UProvinceProxy* InProvince);

	UFUNCTION(BlueprintCallable, Category="Character")
	void JoinOrganization(const UOrganizationProxy* InOrganization);

	/** Destroys the entity. The name and portrait stay behind, and so does this proxy. */
	UFUNCTION(BlueprintCallable, Category="Character")
	void DestroyCharacter();

	/** The facade underneath, for the few calls no proxy covers. */
	UCharacters* GetContainer() const { return Characters; }

protected:
	virtual void BindEvents() override;
	virtual void UnbindEvents() override;

private:
	UFUNCTION()
	void HandleCharacterEvent(FCharacterId InCharacterId);

	FCharacterId CharacterId;

	UPROPERTY()
	TObjectPtr<UCharacters> Characters;
};
