#pragma once

#include "CoreMinimal.h"
#include "BuildingOwner.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Buildings.generated.h"

class UBuildingType;
class UBuildingEntitySubsystem;

class FBuildingsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildingsEvent, FBuildingId, InBuildingId);

/**
 * The UI-facing face of the building data.
 *
 * Identity, location, type and owner live in Mass entities owned by UBuildingEntitySubsystem;
 * this component keeps only the registry mapping an opaque type index back to the actual
 * UBuildingType* (Mass fragments cannot safely hold UObject pointers) and the change events the
 * view models bind to. Every accessor takes the same FBuildingId every other building API uses.
 */
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class BUILDINGS_API UBuildings : public UActorComponent
{
	GENERATED_BODY()
public:
	UBuildings();

	UPROPERTY(BlueprintAssignable)
	FBuildingsEvent OnBuildingAdded;
	/** Fired after a building's entity is gone. */
	UPROPERTY(BlueprintAssignable)
	FBuildingsEvent OnBuildingDestroyed;

	/** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
	UFUNCTION(BlueprintPure)
	int32 Num() const;

	UFUNCTION(BlueprintPure)
	bool IsAlive(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	UBuildingType* GetBuildingType(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	FProvinceId GetProvince(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	FBuildingOwner GetBuildingOwner(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	TArray<UBuildingType*> CollectBuildingsInProvince(FProvinceId InProvinceId) const;

	UFUNCTION(BlueprintPure)
	int32 CountBuildingsOfType(FProvinceId InProvinceId, UBuildingType* InBuildingType) const;

	UFUNCTION(BlueprintPure)
	int32 CountBuildingsInSlot(FProvinceId InProvinceId, FName InSlotName) const;

	/** Refuses and returns an invalid id if InProvinceId is already at InBuildingType's MaxPerProvince. */
	UFUNCTION(BlueprintCallable)
	FBuildingId Add(UBuildingType* InBuildingType, FProvinceId InProvinceId, const FBuildingOwner& InOwner);

	UFUNCTION(BlueprintCallable)
	void Destroy(FBuildingId InBuildingId);

private:
	UBuildingEntitySubsystem* GetEntities() const;

	/** TypeIndex -> asset. Find-or-add registry; never shrinks. */
	int32 ResolveTypeIndex(UBuildingType* InBuildingType);

	UPROPERTY()
	TArray<TObjectPtr<UBuildingType>> BuildingTypes;
};
