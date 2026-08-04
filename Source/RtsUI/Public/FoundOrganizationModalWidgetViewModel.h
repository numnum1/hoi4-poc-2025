#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "FoundOrganizationModalWidgetViewModel.generated.h"

class UBuildingKindViewModel;

/**
 * The found-organization modal's own component, in exactly the sense the "ViewModels sind
 * Komponenten" rule in CLAUDE.md means it — but unlike a list row (UActionViewModel,
 * UBuildingKindViewModel, ...), this one is CreateInstance: the widget's own Blueprint spawns it,
 * it is never constructed or cached from C++. UPlayerCharacterViewModel does not know this class
 * exists.
 *
 * Every property here is therefore just one half of a binding pair the widget's Blueprint wires
 * up itself: a one-way Function binding from UPlayerCharacterViewModel's matching Get (e.g.
 * GetHeadquartersTypes -> SetHeadquartersTypes) for everything the modal only displays, and a
 * genuine Two Way Function binding for OrganizationName (Get/SetOrganizationName here <->
 * GetPendingOrganizationName/SetPendingOrganizationName there) — typing calls SetOrganizationName,
 * which writes the property like any other so the widget sees it immediately, and the same edit
 * also flows to UPlayerCharacterViewModel::SetPendingOrganizationName, whose own re-validation
 * writes back down through the same binding on the next tick. Widgets must still resolve
 * UPlayerCharacterViewModel to bind against (UFoundOrganizationModalWidget::GetPlayerViewModel())
 * — this class only carries the mirrored values, it never reaches for the parent itself.
 *
 * Confirm/Cancel stay plain click events (OnConfirmClicked/OnCancelClicked), the same shape every
 * other row in this codebase reports a click with — a two-way property does not fit a button.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UFoundOrganizationModalWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Two-way binding target: writes locally (so the widget sees the keystroke at once) and is
	 * also the destination half of a Two Way Function binding to
	 * UPlayerCharacterViewModel::PendingOrganizationName, whose re-validated value is what actually
	 * comes back down the same binding. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetOrganizationName(const FText& InName);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetHeadquartersTypes(const TArray<UBuildingKindViewModel*>& InHeadquartersTypes);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetSelectedHeadquartersType(UBuildingKindViewModel* InSelectedHeadquartersType);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetSelectedProvinceName(const FText& InSelectedProvinceName);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetPendingCost(int32 InPendingCost);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetModalErrorText(const FText& InModalErrorText);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetCanConfirm(bool bInCanConfirm);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmClicked);
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnConfirmClicked OnConfirmClicked;

	/** Bind the confirm button's OnClicked to this. Broadcasts OnConfirmClicked, which the widget's
	 * Blueprint wires (Event binding) to UPlayerCharacterViewModel::ConfirmModal(). */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CallOnConfirmClicked();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelClicked);
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnCancelClicked OnCancelClicked;

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CallOnCancelClicked();

protected:
	/** Written by SetOrganizationName below, the same shape every other flat property here uses —
	 * not a UPROPERTY Setter, so it stays a plain UFUNCTION Blueprint/MVVM can target directly. */
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
