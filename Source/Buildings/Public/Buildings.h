#pragma once

#include "CoreMinimal.h"
#include "BuildingOwnerType.h"
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
 *
 * Ownership has no single "the owner" value at this layer either — AddForCharacter/
 * AddForOrganization and the two Collect* queries mirror the entity side's two mutually
 * exclusive owner fragments, translated to EBuildingOwnerType only for GetOwnerType.
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
	/** Fired after SetCharacterOwner/SetOrganizationOwner changed who a building belongs to. */
	UPROPERTY(BlueprintAssignable)
	FBuildingsEvent OnBuildingOwnerChanged;

	/** Ids ever created, alive or not. Slot count, not a live headcount — see IsAlive. */
	UFUNCTION(BlueprintPure)
	int32 Num() const;

	/** Ids currently alive, in creation order. */
	UFUNCTION(BlueprintPure)
	TArray<FBuildingId> GetAliveBuildings() const;

	UFUNCTION(BlueprintPure)
	bool IsAlive(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	UBuildingType* GetBuildingType(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	FProvinceId GetProvince(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	EBuildingOwnerType GetOwnerType(FBuildingId InBuildingId) const;

	/** Valid iff GetOwnerType() == BOT_Character. */
	UFUNCTION(BlueprintPure)
	FCharacterId GetOwnerCharacter(FBuildingId InBuildingId) const;

	/** Valid iff GetOwnerType() == BOT_Organization. */
	UFUNCTION(BlueprintPure)
	FOrganizationId GetOwnerOrganization(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	TArray<FBuildingId> CollectBuildingsInProvince(FProvinceId InProvinceId) const;

	UFUNCTION(BlueprintPure)
	TArray<FBuildingId> CollectBuildingsOwnedByCharacter(FCharacterId InOwnerId) const;

	UFUNCTION(BlueprintPure)
	TArray<FBuildingId> CollectBuildingsOwnedByOrganization(FOrganizationId InOwnerId) const;

	UFUNCTION(BlueprintCallable)
	FBuildingId AddForCharacter(UBuildingType* InBuildingType, FProvinceId InProvinceId, FCharacterId InOwnerId);

	UFUNCTION(BlueprintCallable)
	FBuildingId AddForOrganization(UBuildingType* InBuildingType, FProvinceId InProvinceId, FOrganizationId InOwnerId);

	UFUNCTION(BlueprintCallable)
	void SetCharacterOwner(FBuildingId InBuildingId, FCharacterId InNewOwnerId);

	UFUNCTION(BlueprintCallable)
	void SetOrganizationOwner(FBuildingId InBuildingId, FOrganizationId InNewOwnerId);

	UFUNCTION(BlueprintCallable)
	void Destroy(FBuildingId InBuildingId);

private:
	UBuildingEntitySubsystem* GetEntities() const;

	/** TypeIndex -> asset. Find-or-add registry; never shrinks. */
	int32 ResolveTypeIndex(UBuildingType* InBuildingType);

	UPROPERTY()
	TArray<TObjectPtr<UBuildingType>> BuildingTypes;
};
