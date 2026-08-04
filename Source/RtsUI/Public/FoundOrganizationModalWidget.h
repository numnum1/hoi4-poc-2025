#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FoundOrganizationModalWidget.generated.h"

class UFoundOrganizationModalWidgetViewModel;

/**
 * The found-organization modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Lives inside the played character's view because it is that character who
 * founds the organization — the modal has no meaning without one.
 *
 * The modal collects three things: a name, which headquarters to put up, and where it goes. Only
 * the name comes through this widget. The headquarters is picked from the view model's
 * HeadquartersTypes rows, which carry their own click; the site is picked by clicking the world map,
 * which is *the* province interaction in this game — there is no province list or dropdown here.
 *
 * Data down: UPlayedCharacterView pushes UPlayerCharacterViewModel::GetFoundOrganizationModalViewModel()
 * in through SetViewModel; this widget binds to that view model only, never to the played character's
 * view model directly and never to a subsystem itself. Events up: the name text box's two-way binding
 * and the Blueprint's two buttons all go through UFoundOrganizationModalWidgetViewModel — CommitName/
 * Confirm/Cancel below only forward to it, they decide nothing. Which name is valid, whether the
 * action may run, what running it does is all decided further up, by UPlayerCharacterViewModel and
 * URtsActions.
 *
 * Display values are not routed through here at all: bind Visibility to
 * UPlayerCharacterViewModel::bIsFoundOrganizationModalOpen, and the error label, the site label, the
 * price and the confirm button's IsEnabled straight to this view model's FieldNotify properties
 * (ModalErrorText, SelectedProvinceName, PendingCost, bCanConfirm) with MVVM.
 */
UCLASS(Abstract)
class RTSUI_API UFoundOrganizationModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by the owning view once it has resolved the played character's view model. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetViewModel(UFoundOrganizationModalWidgetViewModel* InViewModel);

	UFUNCTION(BlueprintPure, Category="Found Organization Modal")
	UFoundOrganizationModalWidgetViewModel* GetViewModel() const { return ViewModel; }

	/** Bind the name text box's OnTextChanged here — the input is controlled, so every edit is
	 * reported upward and the validated value comes back down through the same view model. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CommitName(const FText& InName);

	/** Bind the confirm button here. Safe to call even when the input is invalid: the gate is
	 * re-checked further up and does nothing if it refuses. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Confirm();

	/** Bind the cancel button and any backdrop click here. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Cancel();

private:
	UPROPERTY(Transient)
	TObjectPtr<UFoundOrganizationModalWidgetViewModel> ViewModel;
};
