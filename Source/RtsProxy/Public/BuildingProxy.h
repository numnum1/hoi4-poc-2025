#pragma once

#include "CoreMinimal.h"
#include "BuildingOwnerType.h"
#include "RtsEntityProxy.h"
#include "RtsIds.h"
#include "BuildingProxy.generated.h"

class UBuildings;
class UBuildingType;
class UCharacterProxy;
class UOrganizationProxy;
class UProvinceProxy;

/**
 * One building, as an object you can navigate from instead of an id you have to carry around.
 *
 * Everything is read through UBuildings on each call. Get one from
 * URtsProxySubsystem::GetBuilding, or from any other proxy that leads here. See URtsEntityProxy
 * for the rules all proxies follow.
 *
 * A building is owned by either a character or an organization: GetOwnerCharacter and
 * GetOwnerOrganization each return nullptr when the other one applies, so a caller can simply
 * ask for the kind it can display.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UBuildingProxy : public URtsEntityProxy
{
	GENERATED_BODY()

public:
	/** Called by URtsProxySubsystem right after construction. Not for general use. */
	void Setup(URtsProxySubsystem* InProxies, FBuildingId InBuildingId, UBuildings* InBuildings);

	UFUNCTION(BlueprintPure, Category="Building")
	FBuildingId GetId() const { return BuildingId; }

	virtual bool IsAlive() const override;

	UFUNCTION(BlueprintPure, Category="Building")
	UBuildingType* GetBuildingType() const;

	//~ Navigation -------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Building")
	UProvinceProxy* GetProvince() const;

	UFUNCTION(BlueprintPure, Category="Building|Owner")
	EBuildingOwnerType GetOwnerType() const;

	/** The owning character, or nullptr if an organization owns this building. */
	UFUNCTION(BlueprintPure, Category="Building|Owner")
	UCharacterProxy* GetOwnerCharacter() const;

	/** The owning organization, or nullptr if a character owns this building. */
	UFUNCTION(BlueprintPure, Category="Building|Owner")
	UOrganizationProxy* GetOwnerOrganization() const;

	//~ Mutations --------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category="Building")
	void DestroyBuilding();

	/** The facade underneath, for the few calls no proxy covers. */
	UBuildings* GetContainer() const { return Buildings; }

protected:
	virtual void BindEvents() override;
	virtual void UnbindEvents() override;

private:
	UFUNCTION()
	void HandleBuildingEvent(FBuildingId InBuildingId);

	FBuildingId BuildingId;

	UPROPERTY()
	TObjectPtr<UBuildings> Buildings;
};
