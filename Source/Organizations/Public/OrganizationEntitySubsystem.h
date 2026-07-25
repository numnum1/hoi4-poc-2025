#pragma once

#include "CoreMinimal.h"
#include "OrganizationInfo.h"
#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "OrganizationEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

/**
 * Owns the organization entities and bridges them to the legacy int32 organization index.
 *
 * Same shape as UCharacterEntitySubsystem, with one difference: organizations are not
 * loaded from a scenario, they are founded one at a time while playing. So instead of a
 * batch Create there is a single CreateOrganization returning the index it just claimed.
 * Indices are never reused — FPosition::OrganizationIndex must stay valid for the session.
 */
UCLASS()
class ORGANIZATIONS_API UOrganizationEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Spawns one organization entity and returns its index. */
	int32 CreateOrganization();

	/** Destroys every organization entity. For a scenario reload. */
	void Reset();

	FMassEntityHandle GetEntity(int32 InOrganizationIndex) const
	{
		return EntityByIndex.IsValidIndex(InOrganizationIndex) ? EntityByIndex[InOrganizationIndex] : FMassEntityHandle();
	}

	int32 Num() const { return EntityByIndex.Num(); }

	// --- read ---
	bool IsAlignmentPublished(int32 InOrganizationIndex) const;
	FOrganizationAlignment GetAlignment(int32 InOrganizationIndex) const;
	TArray<int32> GetSettledNations(int32 InOrganizationIndex) const;

	// --- write ---
	void PublishAlignment(int32 InOrganizationIndex, const TArray<float>& InAlignment);
	/** Returns false if that nation was already settled. */
	bool SettleNation(int32 InOrganizationIndex, int32 InNationIndex);

	/** Logs one organization's fragments. Backs the Rts.DumpOrganizations console command. */
	void LogOrganization(int32 InOrganizationIndex) const;

	FMassEntityManager& GetEntityManager() const;

private:
	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	/** Index -> entity. The inverse lives on each entity as FOrganizationIndexFragment. */
	TArray<FMassEntityHandle> EntityByIndex;
};
