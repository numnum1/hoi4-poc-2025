#pragma once

#include "CoreMinimal.h"
#include "BuildingKind.h"
#include "BuildingOwnerType.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "MassEntityQuery.h"
#include "Subsystems/WorldSubsystem.h"
#include "BuildingEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

namespace UE::Mass { struct FEntityBuilder; }

/**
 * Owns the building entities and bridges them to the stable FBuildingId.
 *
 * Same shape as the other domain subsystems. What a building *is* travels with the entity as a
 * plain EBuildingKind (see FBuildingKindFragment); everything derived from it — name, icon, build
 * cost — is answered by UBuildingKinds, not stored here.
 *
 * Ownership is not a stored enum: exactly one of FBuildingCharacterOwnerFragment /
 * FBuildingOrganizationOwnerFragment is present on an entity, and GetOwnerType reads that back
 * from fragment presence. SetCharacterOwner/SetOrganizationOwner move the entity to the other
 * archetype when the owner kind changes.
 */
UCLASS()
class BUILDINGS_API UBuildingEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one building entity owned by a character and returns the id it claims. Ids are
	 * never reused. */
	FBuildingId CreateBuildingForCharacter(FProvinceId InProvinceId, EBuildingKind InKind, FCharacterId InOwnerId);

	/** Spawns one building entity owned by an organization and returns the id it claims. */
	FBuildingId CreateBuildingForOrganization(FProvinceId InProvinceId, EBuildingKind InKind, FOrganizationId InOwnerId);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyBuilding(FBuildingId InBuildingId);

	bool IsAlive(FBuildingId InBuildingId) const { return GetEntity(InBuildingId).IsValid(); }

	FMassEntityHandle GetEntity(FBuildingId InBuildingId) const
	{
		return EntityByIndex.IsValidIndex(InBuildingId.Value) ? EntityByIndex[InBuildingId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FBuildingId>& GetAliveIds() const { return AliveIds; }

	// --- read ---
	FProvinceId GetProvince(FBuildingId InBuildingId) const;
	EBuildingKind GetKind(FBuildingId InBuildingId) const;
	/** Derived from which owner fragment is present. BOT_None if InBuildingId is not alive. */
	EBuildingOwnerType GetOwnerType(FBuildingId InBuildingId) const;
	/** Valid iff GetOwnerType() == BOT_Character. */
	FCharacterId GetOwnerCharacter(FBuildingId InBuildingId) const;
	/** Valid iff GetOwnerType() == BOT_Organization. */
	FOrganizationId GetOwnerOrganization(FBuildingId InBuildingId) const;

	/** Every building currently standing in InProvinceId. */
	TArray<FBuildingId> CollectBuildingsInProvince(FProvinceId InProvinceId) const;
	/** Every building currently owned by InOwnerId. */
	TArray<FBuildingId> CollectBuildingsOwnedByCharacter(FCharacterId InOwnerId) const;
	/** Every building currently owned by InOwnerId. */
	TArray<FBuildingId> CollectBuildingsOwnedByOrganization(FOrganizationId InOwnerId) const;

	/** Repoints the character owner fragment. Moves the entity's archetype if it was previously
	 * organization-owned; a pure value write if it was already character-owned. */
	void SetCharacterOwner(FBuildingId InBuildingId, FCharacterId InNewOwnerId);
	/** Repoints the organization owner fragment. See SetCharacterOwner. */
	void SetOrganizationOwner(FBuildingId InBuildingId, FOrganizationId InNewOwnerId);

	/** Logs one building's fragments. Backs the Rts.DumpBuildings console command. */
	void LogBuilding(FBuildingId InBuildingId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FBuildingIndexFragment. An invalid
	 * handle means that slot's building is gone; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	/** Ids currently alive, in creation order. */
	TArray<FBuildingId> AliveIds;

	/** Shared entity-creation core: stamps the common fragments onto whatever the caller already
	 * added to InOutBuilder (the owner fragment), commits, and registers the id. */
	FBuildingId CreateBuilding(FProvinceId InProvinceId, EBuildingKind InKind, UE::Mass::FEntityBuilder& InOutBuilder);

	/** Standalone chunk-iteration queries. Not tied to a processor — see Initialize for how they
	 * are bound. Mutable: running a query updates internal archetype caches even though the
	 * calling methods are const. */
	mutable FMassEntityQuery LocationQuery;
	mutable FMassEntityQuery CharacterOwnerQuery;
	mutable FMassEntityQuery OrganizationOwnerQuery;
};
