#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapTooltipWidget.generated.h"

class UMapTooltipViewModel;

/**
 * The tooltip shown for whatever the mouse is currently hovering on the world map. C++ half of a
 * widget whose Blueprint child carries only layout and styling; the Blueprint spawns its own
 * UMapTooltipViewModel (CreateInstance) in its View Models panel and pushes it here so the owner
 * can reach it through GetViewModel and call SetText when the hover target changes.
 */
UCLASS(Abstract)
class RTSUI_API UMapTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Map Tooltip")
	void SetViewModel(UMapTooltipViewModel* InViewModel);

	UFUNCTION(BlueprintPure, Category="Map Tooltip")
	UMapTooltipViewModel* GetViewModel() const { return ViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<UMapTooltipViewModel> ViewModel;
};
