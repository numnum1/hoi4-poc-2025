#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "ProvinceViewModel.generated.h"

class UEcology;
class UProvinces;
class USoundBase;

/**
 * A read-only C++ MVVM view model for a single province, addressed by the same FProvinceId
 * every other province API uses. See UCharacterViewModel (Characters module) for the pattern
 * this follows: Initialize(Container, Id), flat FieldNotify properties fed from the container's
 * getters, one shared HandleChanged(Id) bound to every change event.
 *
 * FProvinceDisplayInfo is taken apart into one property per member — UMG binds single values,
 * not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class PROVINCES_API UProvinceViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Province Viewmodel")
	void Initialize(UProvinces* InProvinces, FProvinceId InProvinceId);

protected:
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
	int32 GetTimezoneOffset() const { return TimezoneOffset; }

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Refresh();
	void Unbind();

	UFUNCTION()
	void HandleProvinceEvent(FProvinceId InProvinceId);

	UPROPERTY()
	TObjectPtr<UProvinces> ProvincesContainer;
};
