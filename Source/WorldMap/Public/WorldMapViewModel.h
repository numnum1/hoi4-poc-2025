#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "WorldMapViewModel.generated.h"

/**
 * Widget state for the WeltMap: which province the player currently has selected on it. Nothing
 * here is derived from the simulation — a province only becomes "selected" because the player
 * clicked it, so this is UI-invented state, the same distinction UPlayerCharacterViewModel draws
 * between itself and UCharacterViewModel.
 *
 * SelectedProvinceId is meant to be read as a destination binding by other CreateInstance view
 * models that need a province as context (e.g. a province detail panel's
 * UProvinceViewModel::Initialize), the same "Kind-ViewModels: Create Instance + Initialize"
 * pattern UTimezoneViewModel::Initialize is bound to a character id with.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class WORLDMAP_API UWorldMapViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Bind this to the WeltMap's province click handler. Passing an invalid id clears the
	 * selection. */
	UFUNCTION(BlueprintCallable, Category="World Map Viewmodel")
	void SelectProvince(FProvinceId InProvinceId);

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="World Map Viewmodel", meta=(AllowPrivateAccess))
	FProvinceId SelectedProvinceId;

	/** SelectedProvinceId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="World Map Viewmodel", meta=(AllowPrivateAccess))
	int32 SelectedProvinceIdValue = INDEX_NONE;

public:
	FProvinceId GetSelectedProvinceId() const { return SelectedProvinceId; }
	int32 GetSelectedProvinceIdValue() const { return SelectedProvinceIdValue; }
};
