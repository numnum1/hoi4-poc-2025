#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "FoundOrganizationModalWidgetViewModel.generated.h"

/**
 * The found-organization form. A deliberately dumb component: the only thing it owns is the name
 * the player types, and everything else it shows is pushed in.
 *
 * It used to compute its own headquarters catalogue, affordability and validity. It cannot any
 * more, and should not: judging those needs the played character, their money, the site province
 * and the nation that owns it — contexts URtsHUDViewModel holds and a form spawned inside one
 * modal does not. The building itself is no longer picked here at all; there is one build menu, it
 * lives in the HUD, and this form only displays what was picked in it.
 *
 * Every value below other than OrganizationName therefore arrives through a binding wired in the
 * widget Blueprint, sourced from URtsHUDViewModel — this class holds no reference to it, and no
 * view model here ever holds a reference to another. OrganizationName travels the same way in the
 * other direction, onto URtsHUDViewModel::SetPendingOrganizationName, because the verdict this form
 * displays cannot be computed without it. That makes this a controlled input in the React sense:
 * the field lives here, the judgement lives above.
 *
 * bIsValid is display only, never authority — URtsHUDViewModel re-runs both gates on
 * exactly the values it is handed when confirm is clicked.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UFoundOrganizationModalWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** The one value this form owns. Bind the text box to it, and bind it onward to
	 * URtsHUDViewModel::SetPendingOrganizationName so the verdict can account for it. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetOrganizationName(const FText& InOrganizationName);

	/** Pushed from URtsHUDViewModel::SelectedBuildingName — what the build menu has picked. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetSelectedBuildingName(const FText& InSelectedBuildingName);

	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetSelectedBuildingCost(int32 InSelectedBuildingCost);

	/** Pushed from URtsHUDViewModel::SelectedProvinceName — the site, picked on the world map. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetSelectedProvinceName(const FText& InSelectedProvinceName);

	/** Pushed from URtsHUDViewModel::ComputeFoundOrganizationError. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetErrorText(const FText& InErrorText);

	/** Pushed from URtsHUDViewModel::CanConfirmFoundOrganization. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetIsValid(bool bInIsValid);

	/** Clears the typed name. Nothing tells this form when the modal opens, so call it from the
	 * widget Blueprint whenever the modal becomes visible again — the pushed values need no reset,
	 * they follow whatever the HUD says. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void ResetForm();

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText OrganizationName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText SelectedBuildingName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	int32 SelectedBuildingCost = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText SelectedProvinceName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	FText ErrorText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Found Organization Modal", meta=(AllowPrivateAccess))
	bool bIsValid = false;

public:
	FText GetOrganizationName() const { return OrganizationName; }
	FText GetSelectedBuildingName() const { return SelectedBuildingName; }
	int32 GetSelectedBuildingCost() const { return SelectedBuildingCost; }
	FText GetSelectedProvinceName() const { return SelectedProvinceName; }
	FText GetErrorText() const { return ErrorText; }
	bool IsValid() const { return bIsValid; }
};
