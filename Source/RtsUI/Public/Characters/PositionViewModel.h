#pragma once

#include "CoreMinimal.h"
#include "Characters.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "PositionViewModel.generated.h"

class UOrganizations;
class URoles;

/**
 * One of a character's positions — an organization and the role held there — as a row a UMG
 * ListView can bind directly.
 *
 * Not a single-parameter Initialize(Id): a position has no id of its own, it is identified by
 * (character, index) — see FPosition in Characters.h. SetSource is driven by
 * UCharacterViewModel::RefreshPositions, which owns and caches one instance per index and passes
 * the FPosition, the character holding it, and the two facades needed to display it — this row
 * never resolves anything itself.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UPositionViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Repoints this row at a new position and re-broadcasts every field. */
	void SetSource(const FPosition& InPosition, FCharacterId InHolderId, UOrganizations* InOrganizations, URoles* InRoles);

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Position", meta=(AllowPrivateAccess))
	FOrganizationId OrganizationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Position", meta=(AllowPrivateAccess))
	int32 OrganizationIdValue = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Position", meta=(AllowPrivateAccess))
	FText OrganizationName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Position", meta=(AllowPrivateAccess))
	FSlateBrush OrganizationLogo;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Position", meta=(AllowPrivateAccess))
	FRoleId RoleId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Position", meta=(AllowPrivateAccess))
	int32 RoleIdValue = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Position", meta=(AllowPrivateAccess))
	FText RoleName;

	/** True if the character holding this position also owns the organization — they founded it.
	 * Separate from the role: the role says what they may do, this says the place is theirs. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsOwner, Category="Position", meta=(AllowPrivateAccess))
	bool bIsOwner = false;

public:
	FOrganizationId GetOrganizationId() const { return OrganizationId; }
	int32 GetOrganizationIdValue() const { return OrganizationIdValue; }
	FText GetOrganizationName() const { return OrganizationName; }
	FSlateBrush GetOrganizationLogo() const { return OrganizationLogo; }
	FRoleId GetRoleId() const { return RoleId; }
	int32 GetRoleIdValue() const { return RoleIdValue; }
	FText GetRoleName() const { return RoleName; }
	bool IsOwner() const { return bIsOwner; }
};
