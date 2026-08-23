#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MapTooltip/MapTooltipContent.h"
#include "MapTooltipViewModel.generated.h"

/**
 * The tooltip shown for whatever the mouse is currently hovering on the world map. Pure widget
 * state — nothing here is derived from the simulation, the content is whatever the hovering code
 * decided to show, so this is a CreateInstance component with a single pushed value rather than
 * something that resolves its own facades.
 *
 * SetContent takes FMapTooltipContent as one unit, but Headline/Content stay separate FieldNotify
 * properties below rather than a struct property — UMG cannot bind into a struct.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UMapTooltipViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Map Tooltip")
	void SetContent(const FMapTooltipContent& InContent);

	/** Whether the tooltip has anything to say — bind the widget's visibility to this. Computed
	 * rather than stored so it cannot disagree with what SetContent last pushed in. */
	UFUNCTION(BlueprintPure, FieldNotify, Category="Map Tooltip")
	bool ComputeShow() const;

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Map Tooltip", meta=(AllowPrivateAccess))
	FText Headline;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Map Tooltip", meta=(AllowPrivateAccess))
	FText Content;

public:
	FText GetHeadline() const { return Headline; }
	FText GetContent() const { return Content; }
};
