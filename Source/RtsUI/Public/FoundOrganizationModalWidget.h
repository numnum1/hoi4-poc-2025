#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FoundOrganizationModalWidget.generated.h"

class UPlayerCharacterViewModel;

/**
 * The found-organization modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Lives inside the played character's view because it is that character who
 * founds the organization — the modal has no meaning without one.
 *
 * Data down: UPlayedCharacterView pushes the view model in through SetPlayerViewModel; this widget
 * never resolves it from a subsystem itself. Events up: the Blueprint wires its text box and its
 * two buttons to CommitName/Confirm/Cancel, which only forward — every decision (is the name
 * usable, may the action run, what does running it do) is made by UPlayerCharacterViewModel and
 * URtsActions.
 *
 * Display values are not routed through here at all: bind Visibility, the error label and the
 * confirm button's IsEnabled straight to the view model's FieldNotify properties
 * (bIsFoundOrganizationModalOpen, ModalErrorText, bCanConfirmModal) with MVVM.
 */
UCLASS(Abstract)
class RTSUI_API UFoundOrganizationModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by the owning view once it has its view model. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel);

	UFUNCTION(BlueprintPure, Category="Found Organization Modal")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

	/** Bind the name text box's OnTextChanged here — the input is controlled, so every edit goes
	 * to the view model and the validation comes back as ModalErrorText/bCanConfirmModal. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void CommitName(const FText& InName);

	/** Bind the confirm button here. Safe to call even when the input is invalid: the view model
	 * re-checks both gates and does nothing if either refuses. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Confirm();

	/** Bind the cancel button and any backdrop click here. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Cancel();

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;
};
