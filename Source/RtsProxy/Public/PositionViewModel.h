#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "PositionViewModel.generated.h"

class UCharacterProxy;

/**
 * One of a character's positions — an organization and the role held there — as a row a UMG
 * ListView can bind directly, the same shape UActionViewModel gives an action.
 *
 * Not a single-parameter Initialize(Id): a position has no id of its own, it is identified by
 * (character, index) — see FPosition in Characters.h. SetSource is driven by
 * UCharacterViewModel::RefreshPositions, which owns and caches one instance per index.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSPROXY_API UPositionViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Repoints this row at a new (character, index) pair and re-broadcasts every field. */
	void SetSource(UCharacterProxy* InCharacter, int32 InPositionIndex);

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

public:
	FOrganizationId GetOrganizationId() const { return OrganizationId; }
	int32 GetOrganizationIdValue() const { return OrganizationIdValue; }
	FText GetOrganizationName() const { return OrganizationName; }
	FSlateBrush GetOrganizationLogo() const { return OrganizationLogo; }
	FRoleId GetRoleId() const { return RoleId; }
	int32 GetRoleIdValue() const { return RoleIdValue; }
	FText GetRoleName() const { return RoleName; }
};
