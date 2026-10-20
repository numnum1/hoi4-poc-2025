#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Mass/EntityHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/TopLevelAssetPath.h"
#include "OrganizationFocusEntitySubsystem.generated.h"

class UMassEntitySubsystem;
struct FMassEntityManager;

UCLASS()
class FOCUSES_API UOrganizationFocusEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FTopLevelAssetPath GetCurrentFocus(FOrganizationId InOrganizationId) const;
	FDateTime GetStartedAt(FOrganizationId InOrganizationId) const;
	FTopLevelAssetPath GetPendingEventFocus(FOrganizationId InOrganizationId) const;
	bool IsCompleted(FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus) const;
	TArray<FOrganizationId> CollectOrganizationsWithRunningFocus() const;
	TArray<FOrganizationId> CollectOrganizations() const;

	void SetCurrentFocus(FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus, const FDateTime& InStartedAt);
	FTopLevelAssetPath CompleteCurrentFocus(FOrganizationId InOrganizationId);
	void AddCompletedFocus(FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus);
	void SetPendingEventFocus(FOrganizationId InOrganizationId, const FTopLevelAssetPath& InFocus);

	void LogOrganization(FOrganizationId InOrganizationId) const;

	FMassEntityManager& GetEntityManager() const;

private:
	FMassEntityHandle FindEntity(FOrganizationId InOrganizationId) const;
	FMassEntityHandle FindOrCreateEntity(FOrganizationId InOrganizationId);

	UPROPERTY()
	TObjectPtr<UMassEntitySubsystem> EntitySubsystem;

	TMap<FOrganizationId, FMassEntityHandle> EntityByOrganization;
};
