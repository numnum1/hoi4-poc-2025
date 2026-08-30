#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SelectablePositionViewModel.generated.h"

/**
 * One entry in the player's position picker: an icon, a label, and a click. Nothing else. The row
 * decides nothing: which position it stands for, whether picking it is allowed, and what picking it
 * changes are all decided by UPlayerCharacterViewModel, which owns the selection.
 *
 * Deliberately not UPositionViewModel: that one is the full read-only description of a position
 * (organization, logo, role, ownership) a detail panel binds to, and it carries no click. Making a
 * picker row out of it would push a click event into every character view that only wants to *show*
 * its positions.
 *
 * Selection state is not on here either. The picker highlights against the owner's SelectedPosition,
 * so there is exactly one place that knows what is selected and no per-row copy to keep in sync.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API USelectablePositionViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Pushed by the owning view model: which position this row stands for and how to show it. */
	void SetSource(int32 InPositionIndex, const FText& InDisplayName, const FSlateBrush& InIcon);

	/** The index into the character's positions this row was built from — how the owner maps a
	 * click back to a position without the row knowing anything about the selection. */
	int32 GetPositionIndex() const { return PositionIndex; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPositionClicked, USelectablePositionViewModel*, ClickedPosition);

	/** The owning view model binds one handler here for every row it creates; that handler decides
	 * what the click means. Bind a button's OnClicked to CallOnClicked() below, not to this. */
	UPROPERTY(BlueprintAssignable, Category="Selectable Position")
	FOnPositionClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Selectable Position")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Selectable Position", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Selectable Position", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }

private:
	int32 PositionIndex = INDEX_NONE;
};
