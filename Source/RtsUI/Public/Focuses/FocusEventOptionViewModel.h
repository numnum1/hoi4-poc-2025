#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "FocusEventOptionViewModel.generated.h"

struct FEventOption;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UFocusEventOptionViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void SetSource(int32 InOptionIndex, const FEventOption& InOption);

	int32 GetOptionIndex() const { return OptionIndex; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Event Option", meta=(AllowPrivateAccess))
	int32 OptionIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Event Option", meta=(AllowPrivateAccess))
	FText Text;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Focus Event Option", meta=(AllowPrivateAccess))
	FText Tooltip;
};
