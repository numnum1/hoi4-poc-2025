#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Styling/SlateBrush.h"
#include "FocusTreeTabViewModel.generated.h"

class UOrganizationFocusTreeAsset;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UFocusTreeTabViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void SetSource(int32 InTreeIndex, const UOrganizationFocusTreeAsset* InTree, bool bInIsActive);

	int32 GetTreeIndex() const { return TreeIndex; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Tree Tab", meta=(AllowPrivateAccess))
	int32 TreeIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Tree Tab", meta=(AllowPrivateAccess))
	FText TreeName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Tree Tab", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Tree Tab", meta=(AllowPrivateAccess))
	bool bIsActive = false;
};
