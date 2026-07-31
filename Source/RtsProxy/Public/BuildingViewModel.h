#pragma once

#include "CoreMinimal.h"
#include "BuildingOwnerType.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "BuildingViewModel.generated.h"

class UBuildingProxy;
class UBuildingType;

/**
 * A read-only C++ MVVM view model for a single building, addressed by the same FBuildingId
 * every other building API uses. See UCharacterViewModel for the pattern this follows:
 * Initialize(Id) takes a single parameter so it stays usable as an MVVM function binding target,
 * fetches the matching UBuildingProxy from URtsProxySubsystem, and refreshes flat FieldNotify
 * properties whenever that proxy reports a change.
 *
 * Ownership is taken apart into one property per member — UMG binds single values, not structs.
 * Only one of OwnerCharacterId/OwnerOrganizationId is meaningful; bIsOwnedByCharacter and
 * bIsOwnedByOrganization exist so a widget can switch on it without an enum comparison node.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSPROXY_API UBuildingViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category="Building Viewmodel")
	void Initialize(FBuildingId InBuildingId);

	//~ Identity ---------------------------------------------------------------------------------

	/** False once the building's entity has been destroyed. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Building Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel", meta=(AllowPrivateAccess))
	FBuildingId BuildingId;

	/** BuildingId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel", meta=(AllowPrivateAccess))
	int32 BuildingIdValue = INDEX_NONE;

	//~ Type and location ------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel", meta=(AllowPrivateAccess))
	TObjectPtr<UBuildingType> BuildingType;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel", meta=(AllowPrivateAccess))
	FProvinceId ProvinceId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel", meta=(AllowPrivateAccess))
	int32 ProvinceIdValue = INDEX_NONE;

	//~ Owner --------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	EBuildingOwnerType OwnerType = EBuildingOwnerType::BOT_None;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsOwnedByCharacter, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	bool bIsOwnedByCharacter = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsOwnedByOrganization, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	bool bIsOwnedByOrganization = false;

	/** Valid iff OwnerType == BOT_Character. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	FCharacterId OwnerCharacterId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	int32 OwnerCharacterIdValue = INDEX_NONE;

	/** Valid iff OwnerType == BOT_Organization. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	FOrganizationId OwnerOrganizationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	int32 OwnerOrganizationIdValue = INDEX_NONE;

public:
	bool IsValid() const { return bIsValid; }
	FBuildingId GetBuildingId() const { return BuildingId; }
	int32 GetBuildingIdValue() const { return BuildingIdValue; }

	UBuildingType* GetBuildingType() const { return BuildingType; }
	FProvinceId GetProvinceId() const { return ProvinceId; }
	int32 GetProvinceIdValue() const { return ProvinceIdValue; }

	EBuildingOwnerType GetOwnerType() const { return OwnerType; }
	bool IsOwnedByCharacter() const { return bIsOwnedByCharacter; }
	bool IsOwnedByOrganization() const { return bIsOwnedByOrganization; }
	FCharacterId GetOwnerCharacterId() const { return OwnerCharacterId; }
	int32 GetOwnerCharacterIdValue() const { return OwnerCharacterIdValue; }
	FOrganizationId GetOwnerOrganizationId() const { return OwnerOrganizationId; }
	int32 GetOwnerOrganizationIdValue() const { return OwnerOrganizationIdValue; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Building Viewmodel")
	void Refresh();

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Unbind();

	UFUNCTION()
	void HandleProxyChanged();

	UPROPERTY()
	TObjectPtr<UBuildingProxy> Proxy;
};
