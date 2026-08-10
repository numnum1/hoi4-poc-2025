#pragma once

#include "CoreMinimal.h"
#include "Resources/ResourceType.h"
#include "MVVMViewModelBase.h"
#include "ResourceDetailViewModel.generated.h"

/**
 * One sub-value of a resource — Money's "Credit", Manpower's "Free", and so on — as a row nested
 * under UResourceViewModel::Details.
 *
 * As dumb as a row can be: display data pushed in by SetSource, nothing else. It resolves nothing
 * itself and has no click of its own — a detail is not an action, it never mutates anything.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UResourceDetailViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Repoints this row at a new detail and re-broadcasts every field. */
	void SetSource(EResourceType InResource, EResourceDetail InDetail, int32 InAmount, const FText& InAmountText);

	EResourceDetail GetDetailType() const { return DetailType; }

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource Detail", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource Detail", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource Detail", meta=(AllowPrivateAccess))
	int32 Amount = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource Detail", meta=(AllowPrivateAccess))
	FText AmountText;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }
	int32 GetAmount() const { return Amount; }
	FText GetAmountText() const { return AmountText; }

private:
	EResourceDetail DetailType = EResourceDetail::RD_Total;
};
