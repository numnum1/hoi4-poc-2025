#pragma once

#include "CoreMinimal.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "SelectableOrganizationViewModel.generated.h"

/**
 * One entry in a list of organizations — the province screen's "who sits here" row, and the join
 * modal's candidate row. Same shape as USelectablePositionViewModel and for the same reason: the
 * row decides nothing, it carries display data and a click, and the owner (whichever view model
 * built the list) decides what the click means.
 *
 * Not UOrganizationViewModel: that one is the full read-only description of an organization (alignment,
 * settled nations, ...) a detail panel binds to, and it resolves its own facades and binds its own
 * events. This row is pushed its two display values and nothing else — it is never itself alive
 * long enough to need refreshing.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSVIEWMODELS_API USelectableOrganizationViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Pushed by the owning view model: which organization this row stands for and how to show it. */
	void SetSource(FOrganizationId InOrganizationId, const FText& InDisplayName, const FSlateBrush& InIcon);

	FOrganizationId GetOrganizationId() const { return OrganizationId; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrganizationClicked, USelectableOrganizationViewModel*, ClickedOrganization);

	/** The owning view model binds one handler here for every row it creates; that handler decides
	 * what the click means. Bind a button's OnClicked to CallOnClicked() below, not to this. */
	UPROPERTY(BlueprintAssignable, Category="Selectable Organization")
	FOnOrganizationClicked OnClicked;

	/** Bind a button's OnClicked to this; it broadcasts OnClicked(this) in turn. */
	UFUNCTION(BlueprintCallable, Category="Selectable Organization")
	void CallOnClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Selectable Organization", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Selectable Organization", meta=(AllowPrivateAccess))
	FSlateBrush Icon;

public:
	FText GetDisplayName() const { return DisplayName; }
	FSlateBrush GetIcon() const { return Icon; }

private:
	FOrganizationId OrganizationId;
};
