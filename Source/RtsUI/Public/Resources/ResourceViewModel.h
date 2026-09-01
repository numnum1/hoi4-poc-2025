#pragma once

#include "CoreMinimal.h"
#include "Resources/ResourceType.h"
#include "Resources/RtsResourceContext.h"
#include "MVVMViewModelBase.h"
#include "ResourceViewModel.generated.h"

class UResourceDetailViewModel;

/**
 * One resource, shown as one row in an owner's resource list — Money, Manpower, Crystals, ...
 * Every resource looks the same at this layer (icon, name, total, a list of sub-values); what
 * backs the numbers is entirely URtsResources::GetAmount's problem, switching per resource. This
 * row never resolves anything itself — see URtsHUDViewModel::RefreshResources, which owns
 * and caches one instance per EResourceType and passes the resource type and the owner context.
 *
 * Concrete, not abstract: there is one hardcoded EResourceType per resource, no per-resource
 * subclass. The row never decides whether it should be expanded — a click only reports itself
 * upward via OnClicked, and URtsHUDViewModel (the one that knows which single resource
 * is currently open) answers by pushing bIsExpanded back down through SetExpanded.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UResourceViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Repoints this row at a new resource for the given owner and re-broadcasts every field. Does
	 * not touch bIsExpanded — that is written only by SetExpanded, so a plain refresh never collapses
	 * a row the owner did not ask to collapse. */
	void SetSource(EResourceType InResource, const FRtsResourceContext& InContext);

	/** Pushed by the owning view model, which is the only one that knows which resource (if any) is
	 * currently the single expanded one. */
	void SetExpanded(bool bInExpanded);

	EResourceType GetResourceType() const { return ResourceType; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceClicked, UResourceViewModel*, ClickedResource);

	/** URtsHUDViewModel binds one handler here for every resource row it creates; that
	 * handler decides which resource ends up expanded and re-pushes SetExpanded across all rows.
	 * Bind a button's OnClicked to CallOnClicked() below, not directly to this delegate. */
	UPROPERTY(BlueprintAssignable, Category="Resource")
	FOnResourceClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Resource")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

	/** RD_Total, pulled out onto the row itself so a widget that only wants the headline number
	 * does not need to reach into Details. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource", meta=(AllowPrivateAccess))
	int32 Amount = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource", meta=(AllowPrivateAccess))
	FText AmountText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UResourceDetailViewModel>> Details;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Resource", meta=(AllowPrivateAccess))
	int32 DetailCount = 0;

	/** Whether Details should currently be shown. Only one resource row is expanded at a time —
	 * enforced by the owner, not by this row. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsExpanded, Category="Resource", meta=(AllowPrivateAccess))
	bool bIsExpanded = false;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }
	int32 GetAmount() const { return Amount; }
	FText GetAmountText() const { return AmountText; }
	const TArray<TObjectPtr<UResourceDetailViewModel>>& GetDetails() const { return Details; }
	int32 GetDetailCount() const { return DetailCount; }
	bool IsExpanded() const { return bIsExpanded; }

private:
	EResourceType ResourceType = EResourceType::RT_Money;

	/** One row per detail index, reused across refreshes rather than reallocated — same idea as
	 * UCharacterViewModel::PositionEntryCache. */
	UPROPERTY()
	TArray<TObjectPtr<UResourceDetailViewModel>> DetailCache;
};
