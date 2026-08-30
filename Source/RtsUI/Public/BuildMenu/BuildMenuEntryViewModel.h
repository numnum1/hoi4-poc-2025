#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "BuildMenuEntryViewModel.generated.h"

class UBuildingType;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UBuildMenuEntryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize(UBuildingType* InBuildingType);

	void SetAffordable(bool bInAffordable);

	void SetSelected(bool bInSelected);

	UBuildingType* GetBuildingType() const { return BuildingType; }

	UFUNCTION(BlueprintCallable, FieldNotify, Category="Build Menu Entry")
	FText ComputeDisplayName() const;

	UFUNCTION(BlueprintCallable, FieldNotify, Category="Build Menu Entry")
	FSlateBrush ComputeIcon() const;

	UFUNCTION(BlueprintCallable, FieldNotify, Category="Build Menu Entry")
	int32 ComputeCost() const;

	UFUNCTION(BlueprintCallable, FieldNotify, Category="Build Menu Entry")
	FText ComputeCostText() const;

	UFUNCTION(BlueprintCallable, FieldNotify, Category="Build Menu Entry")
	bool ComputeIsHeadquarters() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingKindClicked, UBuildMenuEntryViewModel*, ClickedKind);

	UPROPERTY(BlueprintAssignable, Category="Build Menu Entry")
	FOnBuildingKindClicked OnClicked;

	UFUNCTION(BlueprintCallable, Category="Build Menu Entry")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsAffordable, Category="Build Menu Entry", meta=(AllowPrivateAccess))
	bool bIsAffordable = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsSelected, Category="Build Menu Entry", meta=(AllowPrivateAccess))
	bool bIsSelected = false;

public:
	bool IsAffordable() const { return bIsAffordable; }
	bool IsSelected() const { return bIsSelected; }

private:
	UPROPERTY()
	TObjectPtr<UBuildingType> BuildingType;
};
