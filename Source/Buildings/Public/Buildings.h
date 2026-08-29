#pragma once

#include "CoreMinimal.h"
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
 * this component keeps the registry mapping an opaque type index back to the actual
 * UBuildingType* (Mass fragments cannot safely hold UObject pointers) and the change events the
 * view models bind to. Every accessor takes the same FBuildingId every other building API uses.
 *
 * A building always belongs to a character. Organizations do not own buildings — they have a root
 * department, and a department is seated in a building rather than owning it. "Which buildings does
 * this organization use" is therefore a question for UDepartments, not for this component.
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
	/** Fired after SetCharacterOwner changed who a building belongs to. */
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

	/** The character the building belongs to. */
	UFUNCTION(BlueprintPure)
	FCharacterId GetOwnerCharacter(FBuildingId InBuildingId) const;

	UFUNCTION(BlueprintPure)
	TArray<FBuildingId> CollectBuildingsInProvince(FProvinceId InProvinceId) const;

	UFUNCTION(BlueprintPure)
	TArray<FBuildingId> CollectBuildingsOwnedByCharacter(FCharacterId InOwnerId) const;

	/** Puts up a building for InOwnerId. Whoever pays for it owns it — including the headquarters a
	 * character founds an organization in, which stays theirs. */
	UFUNCTION(BlueprintCallable)
	FBuildingId Add(UBuildingType* InBuildingType, FProvinceId InProvinceId, FCharacterId InOwnerId);

	UFUNCTION(BlueprintCallable)
	void SetCharacterOwner(FBuildingId InBuildingId, FCharacterId InNewOwnerId);

	/** Does nothing for a headquarters — see UBuildingType::IsHeadquarters. The rule lives here
	 * rather than in the UI so it holds for every caller, not just for the one that has no button. */
	UFUNCTION(BlueprintCallable)
	void Destroy(FBuildingId InBuildingId);

private:
	UBuildingEntitySubsystem* GetEntities() const;

	/** Find-or-add registry; never shrinks. Filled as buildings are put up — the catalogue of which
	 * types exist is a game rule and lives on the game mode, see IBuildingCatalog. */
	int32 ResolveTypeIndex(UBuildingType* InBuildingType);

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBuildingType>> BuildingTypes;
};
