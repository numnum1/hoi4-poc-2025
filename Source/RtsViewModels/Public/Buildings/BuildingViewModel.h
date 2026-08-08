#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "BuildingViewModel.generated.h"

class UBuildings;
class UBuildingType;

/**
 * A read-only C++ MVVM view model for a single building, addressed by the same FBuildingId
 * every other building API uses. See UCharacterViewModel for the pattern this follows:
 * Initialize(Id) takes a single parameter so it stays usable as an MVVM function binding target,
 * resolves UBuildings from URtsFacadeSubsystem, binds directly to its events (filtered to this
 * id), and refreshes flat FieldNotify properties on every change.
 *
 * A building always belongs to a character, so there is one owner and no owner-kind switch. An
 * organization is never the owner — it has a root department, and a department is only seated in a
 * building; "which organizations sit here" is a UDepartments question, not one for this model.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSVIEWMODELS_API UBuildingViewModel : public UMVVMViewModelBase
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
	FCharacterId OwnerCharacterId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Viewmodel|Owner", meta=(AllowPrivateAccess))
	int32 OwnerCharacterIdValue = INDEX_NONE;

public:
	bool IsValid() const { return bIsValid; }
	FBuildingId GetBuildingId() const { return BuildingId; }
	int32 GetBuildingIdValue() const { return BuildingIdValue; }

	UBuildingType* GetBuildingType() const { return BuildingType; }
	FProvinceId GetProvinceId() const { return ProvinceId; }
	int32 GetProvinceIdValue() const { return ProvinceIdValue; }

	FCharacterId GetOwnerCharacterId() const { return OwnerCharacterId; }
	int32 GetOwnerCharacterIdValue() const { return OwnerCharacterIdValue; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Building Viewmodel")
	void Refresh();

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Unbind();

	UFUNCTION()
	void HandleBuildingEvent(FBuildingId InBuildingId);

	UPROPERTY()
	TObjectPtr<UBuildings> Buildings;
};
