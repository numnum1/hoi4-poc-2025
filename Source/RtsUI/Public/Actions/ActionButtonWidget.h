#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionButtonWidget.generated.h"

/**
 * One action button, as the C++ half of a widget whose Blueprint child carries only layout and
 * styling. There is no view model behind it: every action is known ahead of time, so whoever places
 * one of these (the HUD Blueprint, one fixed instance per action) authors its name
 * and icon right there in the Details panel. Only what moves with the selection is bound —
 * enabled/tooltip/visibility, through the inherited SetIsEnabled/SetToolTipText/SetVisibility
 * against URtsHUDViewModel, instead of this class inventing its own copies of them.
 *
 * OnClicked is a dispatcher rather than a call into URtsHUDViewModel directly, because
 * this widget does not need to know that class exists — only whoever owns it does, and wires
 * OnClicked (through a Custom-Event bridge, a dispatcher cannot be bound directly to a function on
 * another object) onto the matching ClickXxx() there. See "Callbacks gibt es nur auf
 * Manual-ViewModels" in CLAUDE.md — that rule is about ViewModels, not widgets.
 */
UCLASS(Abstract)
class RTSUI_API UActionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionButtonClicked);

	/** Bind (through a Custom-Event bridge) to the owning view model's ClickXxx() for this action. */
	UPROPERTY(BlueprintAssignable, Category="Action Button")
	FOnActionButtonClicked OnClicked;

	/** Bind the button's own OnClicked to this. */
	UFUNCTION(BlueprintCallable, Category="Action Button")
	void CallOnClicked();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action Button", meta=(AllowPrivateAccess, DesignerRebuild))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Action Button", meta=(AllowPrivateAccess, DesignerRebuild))
	FSlateBrush Icon;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }
};
