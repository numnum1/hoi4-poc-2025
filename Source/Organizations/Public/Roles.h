#pragma once

#include "CoreMinimal.h"
#include "RoleInfo.h"
#include "RtsIds.h"
#include "Components/ActorComponent.h"
#include "Roles.generated.h"

/**
 * The runtime library of authored roles — the set a position can carry, addressed by FRoleId.
 * Filled once at scenario load from the URoleAssets the scenario references, then read-only, the
 * same way USchedules holds its authored schedule data.
 *
 * This is not per-character state: the role a character actually holds is the FRoleId on their
 * FPosition, reachable through UCharacters::GetPositions. The library exists so the UI can name
 * that role, check what it grants, and offer the others.
 */
UCLASS(ClassGroup=(Rts), meta=(BlueprintSpawnableComponent))
class ORGANIZATIONS_API URoles : public UActorComponent
{
	GENERATED_BODY()
public:
	URoles();

	/** Replaces the whole library. Called once per scenario load. */
	void Setup(TArray<FRoleInfo> InRoles);

	UFUNCTION(BlueprintPure)
	int32 Num() const { return Values.Num(); }

	UFUNCTION(BlueprintPure)
	bool IsValidRole(FRoleId InRoleId) const { return Values.IsValidIndex(InRoleId.Value); }

	UFUNCTION(BlueprintPure)
	FRoleInfo GetRoleInfo(FRoleId InRoleId) const;

	UFUNCTION(BlueprintPure)
	FText GetRoleName(FRoleId InRoleId) const;

	/** True if the role's granted permissions are a superset of InRequiredPermissions. */
	UFUNCTION(BlueprintPure)
	bool HasPermissions(FRoleId InRoleId, int32 InRequiredPermissions) const;

	/** The first role authored with bIsLeadership set, or an invalid id if none was authored. */
	UFUNCTION(BlueprintPure)
	FRoleId GetLeadershipRole() const;

	FORCEINLINE const TArray<FRoleInfo>& GetRoleInfos() const { return Values; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FRoleInfo> Values;
};
