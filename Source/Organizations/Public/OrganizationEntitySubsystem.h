#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "MassEntityQuery.h"
#include "Subsystems/WorldSubsystem.h"
#include "OrganizationEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the organization entities and bridges them to the stable FOrganizationId.
 *
 * Same shape as UCharacterEntitySubsystem, with one difference: organizations are not
 * loaded from a scenario, they are founded one at a time while playing. So instead of a
 * batch Create there is a single CreateOrganization returning the id it just claimed.
 * Ids are never reused — FPosition::OrganizationId must stay valid for the session.
 */
UCLASS()
class ORGANIZATIONS_API UOrganizationEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one organization entity owned by InOwnerId and returns its id. Ids are never reused.
	 * An invalid owner is legal — a scenario-authored organization has no founder. */
	FOrganizationId CreateOrganization(FCharacterId InOwnerId);

	/** Destroys the entity. The id's slot stays reserved and reports not-alive from then on. */
	void DestroyOrganization(FOrganizationId InOrganizationId);

	bool IsAlive(FOrganizationId InOrganizationId) const { return GetEntity(InOrganizationId).IsValid(); }

	/** Destroys every organization entity. For a scenario reload. */
	void Reset();

	FMassEntityHandle GetEntity(FOrganizationId InOrganizationId) const
	{
		return EntityByIndex.IsValidIndex(InOrganizationId.Value) ? EntityByIndex[InOrganizationId.Value] : FMassEntityHandle();
	}

	/** Every id ever handed out, alive or not. */
	int32 NumSlots() const { return EntityByIndex.Num(); }

	/** Ids currently alive, in creation order. */
	const TArray<FOrganizationId>& GetAliveIds() const { return AliveIds; }

	/** The character this organization belongs to, or an invalid id if it has no owner. */
	FCharacterId GetOwnerCharacter(FOrganizationId InOrganizationId) const;
	/** The department the organization is made of, or an invalid id if it has none. */
	FDepartmentId GetRootDepartment(FOrganizationId InOrganizationId) const;
	bool IsAlignmentPublished(FOrganizationId InOrganizationId) const;
	FOrganizationAlignment GetAlignment(FOrganizationId InOrganizationId) const;
	TArray<FNationId> GetSettledNations(FOrganizationId InOrganizationId) const;
	TArray<FOrganizationId> CollectOrganizationsSettledIn(FNationId InNationId) const;

	/** Points the organization at its root department. Separate from CreateOrganization because the
	 * department is created *for* this organization and so cannot exist before it does. */
	void SetRootDepartment(FOrganizationId InOrganizationId, FDepartmentId InRootDepartmentId);
	void PublishAlignment(FOrganizationId InOrganizationId, const TArray<float>& InAlignment);
	/** Returns false if that nation was already settled. */
	bool SettleNation(FOrganizationId InOrganizationId, FNationId InNationId);

	/** Logs one organization's fragments. Backs the Rts.DumpOrganizations console command. */
	void LogOrganization(FOrganizationId InOrganizationId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Id -> entity. The inverse lives on each entity as FOrganizationIndexFragment. An invalid
	 * handle means that slot's organization is dissolved; the slot itself is never removed. */
	TArray<FMassEntityHandle> EntityByIndex;

	TArray<FOrganizationId> AliveIds;

	/** Standalone chunk-iteration query backing CollectOrganizationsSettledIn. Not tied to a
	 * processor — see UOrganizationEntitySubsystem::Initialize for how it is bound. Mutable:
	 * running it updates internal archetype caches even though the collect call is const. */
	mutable FMassEntityQuery SettledNationsQuery;
};
