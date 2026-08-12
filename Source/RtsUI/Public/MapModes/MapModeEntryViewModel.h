#pragma once

#include "CoreMinimal.h"
#include "MapModes/MapModeType.h"
#include "MVVMViewModelBase.h"
#include "Styling/SlateBrush.h"
#include "MapModeEntryViewModel.generated.h"

/**
 * One map mode, shown as one row in the mode picker. Pure UI data (name, icon, whether it is the
 * active one) plus a click callback — what the mode *is* comes from URtsMapModes, and whether it is
 * active is decided by URtsHUDViewModel, which is the one that knows the current selection.
 *
 * Manual creation because there is one instance per entry in the owning view model's array, which
 * no Blueprint view model entry can produce.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UMapModeEntryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Records which mode this row stands for and pulls its display data out of URtsMapModes.
	 * Active state is not derived here — see SetActive. */
	void SetMapMode(EMapMode InMapMode);

	/** Pushed by the owning view model whenever the active mode changes. */
	void SetActive(bool bInActive);

	EMapMode GetMapMode() const { return MapMode; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapModeClicked, UMapModeEntryViewModel*, ClickedMapMode);

	/** URtsHUDViewModel binds one handler here for every row it creates; that handler switches the
	 * mode. Bind a button's OnClicked to CallOnClicked() below, not directly to this delegate. */
	UPROPERTY(BlueprintAssignable, Category="Map Mode")
	FOnMapModeClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Map Mode")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Map Mode", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Map Mode", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsActive, Category="Map Mode", meta=(AllowPrivateAccess))
	bool bIsActive = false;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }
	bool IsActive() const { return bIsActive; }

private:
	EMapMode MapMode = EMapMode::EMM_Political;
};
