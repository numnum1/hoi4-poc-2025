#pragma once

#include "CoreMinimal.h"
#include "ResourceType.h"
#include "RtsResourceContext.h"
#include "MVVMViewModelBase.h"
#include "ResourceViewModel.generated.h"

class UResourceDetailViewModel;

/**
 * One resource, shown as one row in an owner's resource list — Money, Manpower, Crystals, ...
 * Every resource looks the same at this layer (icon, name, total, a list of sub-values); what
 * backs the numbers is entirely URtsResources::GetAmount's problem, switching per resource. This
 * row never resolves anything itself — see UPlayerCharacterViewModel::RefreshResources, which owns
 * and caches one instance per EResourceType and passes the resource type and the owner context.
 *
 * Concrete, not abstract: there is one hardcoded EResourceType per resource, no per-resource
 * subclass. No OnClicked — a resource row is pure display, it never mutates anything.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSVIEWMODELS_API UResourceViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Repoints this row at a new resource for the given owner and re-broadcasts every field. */
	void SetSource(EResourceType InResource, const FRtsResourceContext& InContext);

	EResourceType GetResourceType() const { return ResourceType; }

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

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }
	int32 GetAmount() const { return Amount; }
	FText GetAmountText() const { return AmountText; }
	const TArray<TObjectPtr<UResourceDetailViewModel>>& GetDetails() const { return Details; }
	int32 GetDetailCount() const { return DetailCount; }

private:
	EResourceType ResourceType = EResourceType::RT_Money;

	/** One row per detail index, reused across refreshes rather than reallocated — same idea as
	 * UCharacterViewModel::PositionEntryCache. */
	UPROPERTY()
	TArray<TObjectPtr<UResourceDetailViewModel>> DetailCache;
};
