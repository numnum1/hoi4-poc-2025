#pragma once

#include "CoreMinimal.h"
#include "RtsEntityProxy.h"
#include "RtsIds.h"
#include "ProvinceProxy.generated.h"

class UBuildingProxy;
class UCharacterProxy;
class UEcology;
class UNationProxy;
class UProvinces;
class USoundBase;

/**
 * One province, as an object you can navigate from instead of an id you have to carry around.
 *
 * Everything is read through UProvinces on each call. Get one from
 * URtsProxySubsystem::GetProvince, or from any other proxy that leads here. See URtsEntityProxy
 * for the rules all proxies follow.
 */
UCLASS(BlueprintType)
class RTSPROXY_API UProvinceProxy : public URtsEntityProxy
{
	GENERATED_BODY()

public:
	/** Called by URtsProxySubsystem right after construction. Not for general use. */
	void Setup(URtsProxySubsystem* InProxies, FProvinceId InProvinceId, UProvinces* InProvinces);

	UFUNCTION(BlueprintPure, Category="Province")
	FProvinceId GetId() const { return ProvinceId; }

	virtual bool IsAlive() const override;

	//~ Display ----------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Province")
	FText GetDisplayName() const;

	/** The name the level uses for this province. */
	UFUNCTION(BlueprintPure, Category="Province")
	FName GetMapId() const;

	UFUNCTION(BlueprintPure, Category="Province")
	FSlateBrush GetPicture() const;

	UFUNCTION(BlueprintPure, Category="Province")
	USoundBase* GetInspectionSound() const;

	UFUNCTION(BlueprintPure, Category="Province")
	UEcology* GetEcology() const;

	/** Hours offset from game time, read from FProvinceTimeFragment. */
	UFUNCTION(BlueprintPure, Category="Province")
	int32 GetTimezoneOffset() const;

	//~ Navigation -------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category="Province")
	UNationProxy* GetOwner() const;

	UFUNCTION(BlueprintPure, Category="Province")
	TArray<UCharacterProxy*> GetCharacters() const;

	/** Every building standing here. Walks all buildings — fine for a UI panel. */
	UFUNCTION(BlueprintPure, Category="Province")
	TArray<UBuildingProxy*> GetBuildings() const;

	//~ Actions ----------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category="Province")
	void SetOwner(const UNationProxy* InNation);

	/** Destroys the entity. The display row stays behind, and so does this proxy. */
	UFUNCTION(BlueprintCallable, Category="Province")
	void DestroyProvince();

	/** The facade underneath, for the few calls no proxy covers. */
	UProvinces* GetContainer() const { return Provinces; }

protected:
	virtual void BindEvents() override;
	virtual void UnbindEvents() override;

private:
	UFUNCTION()
	void HandleProvinceEvent(FProvinceId InProvinceId);

	FProvinceId ProvinceId;

	UPROPERTY()
	TObjectPtr<UProvinces> Provinces;
};
