#pragma once

#include "CoreMinimal.h"
#include "Focuses/FocusTreeLayoutCell.h"
#include "Focuses/OrganizationFocusState.h"
#include "MVVMViewModelBase.h"
#include "Styling/SlateBrush.h"
#include "FocusNodeViewModel.generated.h"

class UOrganizationFocusAsset;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UFocusNodeViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void SetSource(UOrganizationFocusAsset* InFocus, const FFocusTreeLayoutCell& InCell);

	void SetStatus(EOrganizationFocusState InState, const FText& InStateReason, float InProgress);

	UOrganizationFocusAsset* GetFocus() const { return Focus; }
	int32 GetGridRow() const { return GridRow; }
	float GetGridColumn() const { return GridColumn; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	TObjectPtr<UOrganizationFocusAsset> Focus;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	FText FocusName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	FText Tooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	int32 DurationDays = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	EOrganizationFocusState State = EOrganizationFocusState::OFS_Unavailable;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	FText StateReason;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	float Progress = 0.f;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	int32 GridRow = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Node", meta=(AllowPrivateAccess))
	float GridColumn = 0.f;
};
