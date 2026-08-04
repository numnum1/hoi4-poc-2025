#pragma once

#include "CoreMinimal.h"
#include "BuildingKind.h"
#include "BuildingOwnerType.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "Buildings.generated.h"

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
 * Identity, location, kind and owner live in Mass entities owned by UBuildingEntitySubsystem;
 * this component keeps only the change events the view models bind to. There is no type registry
 * any more — a building's kind is the plain EBuildingKind on the entity, and UBuildingKinds turns
 * it into display data and costs. Every accessor takes the same FBuildingId every other building
 * API uses.
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
	EBuildingKind GetKind(FBuildingId InBuildingId) const;

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
	FBuildingId AddForCharacter(EBuildingKind InKind, FProvinceId InProvinceId, FCharacterId InOwnerId);

	UFUNCTION(BlueprintCallable)
	FBuildingId AddForOrganization(EBuildingKind InKind, FProvinceId InProvinceId, FOrganizationId InOwnerId);

	UFUNCTION(BlueprintCallable)
	void SetCharacterOwner(FBuildingId InBuildingId, FCharacterId InNewOwnerId);

	UFUNCTION(BlueprintCallable)
	void SetOrganizationOwner(FBuildingId InBuildingId, FOrganizationId InNewOwnerId);

	/** Does nothing for a headquarters — see UBuildingKinds::IsHeadquarters. The rule lives here
	 * rather than in the UI so it holds for every caller, not just for the one that has no button. */
	UFUNCTION(BlueprintCallable)
	void Destroy(FBuildingId InBuildingId);

private:
	UBuildingEntitySubsystem* GetEntities() const;
};
