#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "FoundOrganizationModalWidgetViewModel.generated.h"

class UBuildingKindViewModel;

/**
 * The found-organization modal's own component, in exactly the sense the "ViewModels sind
 * Komponenten" rule in CLAUDE.md means it: UPlayerCharacterViewModel owns a single cached
 * instance and pushes every value on it from RefreshFoundOrganizationModal. This class invents
 * nothing itself — not even the selection, that stays on the parent as PendingHeadquartersType/
 * SelectedProvinceId — it only carries what the parent last pushed, plus the events a click or a
 * keystroke needs to report upward.
 *
 * OrganizationName is fully controlled, the same way a React controlled input is: its value is a
 * pushed prop (SetSource), never written locally by SetOrganizationName. Typing calls
 * SetOrganizationName, which does nothing but broadcast OnOrganizationNameChanged; the parent
 * re-validates the keystroke (UPlayerCharacterViewModel::SetPendingOrganizationName already does)
 * and the authoritative value comes back down through the next SetSource. Bind the widget's text
 * box two-way to OrganizationName and the parent's PendingOrganizationName two-way to
 * OrganizationName in the same way, and a keystroke flows widget -> here -> parent -> here ->
 * widget, never trusting an intermediate hop as the truth.
 *
 * Confirm/Cancel stay plain click events (OnConfirmClicked/OnCancelClicked), the same shape every
 * other row in this codebase reports a click with (UActionViewModel, UBuildingKindViewModel,
 * USelectablePositionViewModel) — a two-way property does not fit a button.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="Manual"))
class RTSUI_API UFoundOrganizationModalWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Pushed by UPlayerCharacterViewModel::RefreshFoundOrganizationModal whenever any of the
	 * modal's derived state changes — every value here, OrganizationName included. */
	void SetSource(const FText& InOrganizationName,
	               const TArray<TObjectPtr<UBuildingKindViewModel>>& InHeadquartersTypes,
	               UBuildingKindViewModel* InSelectedHeadquartersType,
	               const FText& InSelectedProvinceName,
	               int32 InPendingCost,
	               const FText& InModalErrorText,
	               bool bInCanConfirm);

	/** Bind the name text box's two-way binding target to this. Never writes OrganizationName
	 * itself — see the class comment — only reports the edit upward. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetOrganizationName(const FText& InName);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrganizationNameChanged, FText, NewName);
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnOrganizationNameChanged OnOrganizationNameChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmClicked);
	/** UPlayerCharacterViewModel binds one handler here at creation; that handler runs
	 * ConfirmModal(). Bind the confirm button's OnClicked to CallOnConfirmClicked() below. */
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnConfirmClicked OnConfirmClicked;

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CallOnConfirmClicked();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelClicked);
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnCancelClicked OnCancelClicked;

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CallOnCancelClicked();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText OrganizationName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UBuildingKindViewModel>> HeadquartersTypes;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	int32 HeadquartersTypeCount = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	TObjectPtr<UBuildingKindViewModel> SelectedHeadquartersType;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText SelectedProvinceName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	int32 PendingCost = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText ModalErrorText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=CanConfirm, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	bool bCanConfirm = false;

public:
	FText GetOrganizationName() const { return OrganizationName; }
	const TArray<TObjectPtr<UBuildingKindViewModel>>& GetHeadquartersTypes() const { return HeadquartersTypes; }
	int32 GetHeadquartersTypeCount() const { return HeadquartersTypeCount; }
	UBuildingKindViewModel* GetSelectedHeadquartersType() const { return SelectedHeadquartersType; }
	FText GetSelectedProvinceName() const { return SelectedProvinceName; }
	int32 GetPendingCost() const { return PendingCost; }
	FText GetModalErrorText() const { return ModalErrorText; }
	bool CanConfirm() const { return bCanConfirm; }
};
