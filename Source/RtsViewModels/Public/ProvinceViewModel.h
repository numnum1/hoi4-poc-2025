#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "ProvinceViewModel.generated.h"

class UEcology;
class UNations;
class UProvinces;
class USoundBase;

/**
 * A read-only C++ MVVM view model for a single province, addressed by the same FProvinceId
 * every other province API uses. See UCharacterViewModel for the pattern this follows:
 * Initialize(Id) takes a single parameter so it stays usable as an MVVM function binding target,
 * resolves UProvinces (and UNations, for the owner's name) from URtsFacadeSubsystem, binds
 * directly to UProvinces' events (filtered to this id), and refreshes flat FieldNotify
 * properties on every change.
 *
 * FProvinceDisplayInfo is taken apart into one property per member — UMG binds single values,
 * not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSVIEWMODELS_API UProvinceViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category="Province Viewmodel")
	void Initialize(FProvinceId InProvinceId);

	//~ Identity ---------------------------------------------------------------------------------

	/** False once the province's entity has been destroyed. Display keeps its last known values. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	FProvinceId ProvinceId;

	/** ProvinceId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	int32 ProvinceIdValue = INDEX_NONE;

	//~ FProvinceDisplayInfo ---------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	FName MapId = NAME_None;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	FText ProvinceName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	FSlateBrush Picture;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	TObjectPtr<USoundBase> InspectionSound;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	TObjectPtr<UEcology> Ecology;

	//~ Entity state -----------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	FNationId OwningNationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	int32 OwningNationIdValue = INDEX_NONE;

	/** Read through UNations directly — Provinces itself is not allowed to know that module. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	FText OwningNationName;

	/** Hours offset from game time, read from FProvinceTimeFragment. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	int32 TimezoneOffset = 0;

public:
	bool IsValid() const { return bIsValid; }
	FProvinceId GetProvinceId() const { return ProvinceId; }
	int32 GetProvinceIdValue() const { return ProvinceIdValue; }

	FName GetMapId() const { return MapId; }
	FText GetProvinceName() const { return ProvinceName; }
	FSlateBrush GetPicture() const { return Picture; }
	USoundBase* GetInspectionSound() const { return InspectionSound; }
	UEcology* GetEcology() const { return Ecology; }

	FNationId GetOwningNationId() const { return OwningNationId; }
	int32 GetOwningNationIdValue() const { return OwningNationIdValue; }
	FText GetOwningNationName() const { return OwningNationName; }
	int32 GetTimezoneOffset() const { return TimezoneOffset; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Province Viewmodel")
	void Refresh();

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Unbind();

	UFUNCTION()
	void HandleProvinceEvent(FProvinceId InProvinceId);

	UPROPERTY()
	TObjectPtr<UProvinces> Provinces;

	UPROPERTY()
	TObjectPtr<UNations> Nations;
};
