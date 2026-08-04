#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "BuildingKindViewModel.generated.h"

class UBuildingType;

/**
 * One building type, shown as one row in a picker: name, icon, price tag, and a click. The same
 * shape USelectablePositionViewModel has, and for the same reason — the row decides nothing. What a
 * type costs and what it is called are authored on the UBuildingType asset itself; whether the
 * player can afford it and whether it is the selected one are *pushed in* by the owning view model,
 * which is the one that knows the character's money and the current selection.
 *
 * bIsAffordable and bIsSelected are therefore display values, never the truth: the owner re-derives
 * both when the click comes back up, exactly as UPlayerCharacterViewModel::HandleActionClicked does
 * rather than trusting the last pushed bIsEnabled.
 *
 * Written against UBuildingType in general, not against headquarters: the found-organization modal
 * is the first user, and the same row carries the general build menu once more types are catalogued.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSVIEWMODELS_API UBuildingKindViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Which type this row stands for. Name, icon and cost are read straight off the asset. */
	void SetSource(UBuildingType* InBuildingType);

	/** Pushed by the owning view model, which is the one that knows the buyer's funds. */
	void SetAffordable(bool bInAffordable);

	/** Pushed by the owning view model, which owns the selection. */
	void SetSelected(bool bInSelected);

	UBuildingType* GetBuildingType() const { return BuildingType; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingKindClicked, UBuildingKindViewModel*, ClickedKind);

	/** The owning view model binds one handler here for every row it creates; that handler decides
	 * what the click means. Bind a button's OnClicked to CallOnClicked() below, not to this. */
	UPROPERTY(BlueprintAssignable, Category="Building Kind")
	FOnBuildingKindClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Building Kind")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Kind", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Kind", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Kind", meta=(AllowPrivateAccess))
	int32 Cost = 0;

	/** Cost preformatted, so a widget can bind a label without a conversion node. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Building Kind", meta=(AllowPrivateAccess))
	FText CostText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsAffordable, Category="Building Kind", meta=(AllowPrivateAccess))
	bool bIsAffordable = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsSelected, Category="Building Kind", meta=(AllowPrivateAccess))
	bool bIsSelected = false;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }
	int32 GetCost() const { return Cost; }
	FText GetCostText() const { return CostText; }
	bool IsAffordable() const { return bIsAffordable; }
	bool IsSelected() const { return bIsSelected; }

private:
	UPROPERTY()
	TObjectPtr<UBuildingType> BuildingType;
};
