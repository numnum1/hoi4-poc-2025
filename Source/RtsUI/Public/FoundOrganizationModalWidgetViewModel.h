#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "RtsIds.h"
#include "FoundOrganizationModalWidgetViewModel.generated.h"

class UBanking;
class UBuildingKindViewModel;
class UBuildings;
class UBuildingType;

/**
 * The found-organization modal's own component, in exactly the sense the "ViewModels sind
 * Komponenten" rule in CLAUDE.md means it — but unlike a list row (UActionViewModel,
 * UBuildingKindViewModel, ...), this one is CreateInstance: the widget's own Blueprint spawns it,
 * it is never constructed or cached from C++. UPlayerCharacterViewModel does not know this class
 * exists.
 *
 * Unlike most CreateInstance components in this codebase, this one is a "smart form": the name
 * typed, the headquarters picked, the headquarters catalogue (with affordability) and whether the
 * form is valid right now are all local state and business logic this class owns and computes
 * itself, not values pushed down from a parent. That is deliberate — this form is not a reusable
 * shape another screen will ever bind against, so there is no cost to it knowing exactly what
 * founding an organization requires. Bind Initialize(FCharacterId) from
 * UPlayerCharacterViewModel::CharacterId (destination binding) so it can resolve its own facades and
 * compute the acting character's funds; the province is the one input it cannot invent itself (it is
 * picked on the world map, see CLAUDE.md), so that alone is still pushed in from the parent.
 *
 * OnConfirmClicked reports everything the form collected as parameters, once. Nothing here ever
 * writes into UPlayerCharacterViewModel — the parent's handler builds an FRtsActionContext, re-runs
 * URtsActions::GetBlockedReason/GetInvalidInputReason on exactly what the event carried (this form's
 * own IsValid is UX feedback, not the authority — the same "never trust the last pushed state" rule
 * every other click handler in this codebase follows), and only then executes.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UFoundOrganizationModalWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Bound in Blueprint as a Destination binding from UPlayerCharacterViewModel::CharacterId.
	 * Resolves this form's own facades (Buildings, Banking), rebuilds the headquarters catalogue and
	 * binds to balance changes so affordability stays live while the modal is open. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Initialize(FCharacterId InCharacterId);

	/** Pushed from UPlayerCharacterViewModel — the province is picked on the world map, not typed
	 * into this form, so it arrives the same way every other externally-picked value would. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetSelectedProvinceId(FProvinceId InSelectedProvinceId);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetSelectedProvinceName(const FText& InSelectedProvinceName);

	/** Local form state — never mirrored up to UPlayerCharacterViewModel. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetOrganizationName(const FText& InName);

	/** Clears the form back to its opening state — typed name, headquarters pick. This form does not
	 * reset itself on open: nothing tells it when that happens, the same reason it does not push
	 * anything into the parent. Call this from the widget's Blueprint whenever the modal becomes
	 * visible again. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void ResetForm();

	// Testweise ohne Parameter — siehe Chat-Verlauf: das Event-Binding im Blueprint auf
	// UPlayerCharacterViewModel::HandleFoundOrganizationConfirmed(Name, HeadquartersType, ProvinceId)
	// ließ sich nicht herstellen. Sobald das geklärt ist, wieder auf die Drei-Parameter-Version zurück.
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmClicked);
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnConfirmClicked OnConfirmClicked;

	/** Bind the confirm button's OnClicked to this. Re-checks IsValid before broadcasting — the form
	 * does not trust its own last-pushed bIsValid any more than a parent would trust a child's. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CallOnConfirmClicked();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelClicked);
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnCancelClicked OnCancelClicked;

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CallOnCancelClicked();

protected:
	virtual void BeginDestroy() override;

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

	/** Computed by Revalidate() from the local state above, in the same order
	 * URtsActions::GetInvalidInputReason checks it — kept in sync by hand since that logic lives in
	 * RtsViewModels and cannot be called from here without the full FRtsActionContext/FRtsActionInput
	 * the parent alone can build (funds/context aside, this form has no organization-action gate to
	 * run — GetBlockedReason for EA_FoundOrganization is always empty at RAS_Self). */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText ModalErrorText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	bool bIsValid = false;

public:
	FText GetOrganizationName() const { return OrganizationName; }
	const TArray<TObjectPtr<UBuildingKindViewModel>>& GetHeadquartersTypes() const { return HeadquartersTypes; }
	int32 GetHeadquartersTypeCount() const { return HeadquartersTypeCount; }
	UBuildingKindViewModel* GetSelectedHeadquartersType() const { return SelectedHeadquartersType; }
	FText GetSelectedProvinceName() const { return SelectedProvinceName; }
	int32 GetPendingCost() const { return PendingCost; }
	FText GetModalErrorText() const { return ModalErrorText; }
	bool IsValid() const { return bIsValid; }

private:
	/** The one writer of PendingHeadquartersType-equivalent state here (SelectedHeadquartersType),
	 * same reason UPlayerCharacterViewModel::ApplyHeadquartersType is the one writer there. */
	void ApplyHeadquartersType(UBuildingType* InBuildingType);

	/** Rebuilds HeadquartersTypes with current affordability and re-derives PendingCost/validity. */
	void RefreshHeadquartersTypes();

	/** Re-derives ModalErrorText/bIsValid from the local state, in the order the player fills it in. */
	void Revalidate();

	int32 GetCharacterFunds() const;

	UFUNCTION()
	void HandleHeadquartersTypeClicked(UBuildingKindViewModel* ClickedType);

	UFUNCTION()
	void HandleBalanceChanged(FBankAccountId InAccountId);

	FCharacterId CharacterId;
	FProvinceId SelectedProvinceId;

	UPROPERTY()
	TObjectPtr<UBuildingType> PendingHeadquartersType;

	UPROPERTY()
	TObjectPtr<UBuildings> Buildings;

	/** Resolved by Initialize, bound to for live affordability; unbound in BeginDestroy. */
	UPROPERTY()
	TObjectPtr<UBanking> Banking;

	/** Same idea as UPlayerCharacterViewModel::HeadquartersTypeCache — a rebuild that offers the same
	 * types keeps the same row objects. */
	UPROPERTY()
	TMap<TObjectPtr<UBuildingType>, TObjectPtr<UBuildingKindViewModel>> HeadquartersTypeCache;
};
