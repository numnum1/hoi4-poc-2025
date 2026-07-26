// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RolePermission.h"
#include "Engine/DataAsset.h"
#include "RoleAsset.generated.h"

/**
 * A human-authored role, referenced by the UScenario's Roles list. ScenarioLoader converts this
 * to an FRoleInfo and hands it to URoles::Setup — see UOrganizationAsset for the same authoring
 * pattern. Deliberately holds no Organizations types: the conversion into FRoleInfo happens in
 * ScenarioLoading.h, the one place allowed to know both modules.
 */
UCLASS()
class SCENARIO_API URoleAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	URoleAsset();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText RoleName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, Bitmask, BitmaskEnum="/Script/RtsCore.ERolePermission"))
	int32 Permissions;

	/** Whether founding an organization should hand the founder this role. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bIsLeadership;
public:
	FORCEINLINE const FText& GetRoleName() const { return RoleName; }
	FORCEINLINE int32 GetPermissions() const { return Permissions; }
	FORCEINLINE bool IsLeadership() const { return bIsLeadership; }
};
