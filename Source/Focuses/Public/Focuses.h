#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "UObject/TopLevelAssetPath.h"
#include "Focuses.generated.h"

class UOrganizationFocusAsset;
class UOrganizationFocusEntitySubsystem;
class UOrganizationFocusTreeAsset;

class FFocusesModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOrganizationFocusEvent, FOrganizationId, InOrganizationId);

UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class FOCUSES_API UOrganizationFocuses : public UActorComponent
{
	GENERATED_BODY()

public:
	UOrganizationFocuses();

	UPROPERTY(BlueprintAssignable)
	FOrganizationFocusEvent OnFocusChanged;

	UPROPERTY(BlueprintAssignable)
	FOrganizationFocusEvent OnPendingEventChanged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UOrganizationFocusTreeAsset>> Trees;

	TMap<FTopLevelAssetPath, const UOrganizationFocusAsset*> FocusByPath;

	TMap<const UOrganizationFocusAsset*, const UOrganizationFocusTreeAsset*> TreeByFocus;

public:
	void Setup(const TArray<UOrganizationFocusTreeAsset*>& InTrees);

	const TArray<TObjectPtr<UOrganizationFocusTreeAsset>>& GetTrees() const { return Trees; }

	const UOrganizationFocusTreeAsset* FindTreeOf(const UOrganizationFocusAsset* InFocus) const;

	const UOrganizationFocusAsset* GetCurrentFocus(FOrganizationId InOrganizationId) const;

	FDateTime GetStartedAt(FOrganizationId InOrganizationId) const;

	bool IsCompleted(FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus) const;

	const UOrganizationFocusAsset* GetPendingEventFocus(FOrganizationId InOrganizationId) const;

	TArray<FOrganizationId> CollectOrganizationsWithRunningFocus() const;

	void StartFocus(FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus, const FDateTime& InNow);

	const UOrganizationFocusAsset* CompleteCurrentFocus(FOrganizationId InOrganizationId);

	void CompleteFocusImmediately(FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus);

	void SetPendingEvent(FOrganizationId InOrganizationId, const UOrganizationFocusAsset* InFocus);

	void ClearPendingEvent(FOrganizationId InOrganizationId);

private:
	UOrganizationFocusEntitySubsystem* GetEntities() const;

	const UOrganizationFocusAsset* FindFocus(const FTopLevelAssetPath& InPath) const;

	void ValidateTree(const UOrganizationFocusTreeAsset* InTree) const;
};
