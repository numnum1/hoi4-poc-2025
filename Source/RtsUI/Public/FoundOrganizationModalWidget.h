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
 * This widget binds its own elements to its own component, UFoundOrganizationModalWidgetViewModel
 * — a CreateInstance view model the widget's Blueprint spawns itself in its View Models panel, not
 * something pushed in from C++. What this class still pushes in (SetPlayerViewModel) is the played
 * character's view model, kept around only as a Property Path source: the Blueprint wires one-way
 * Function bindings from UPlayerCharacterViewModel's getters (HeadquartersTypes, PendingCost,
 * SelectedProvinceName, ModalErrorText, CanConfirmModal, ...) onto the modal view model's matching
 * setters, and a Two Way Function binding between PendingOrganizationName/SetPendingOrganizationName
 * there and OrganizationName/SetOrganizationName here. Confirm/Cancel on the modal view model are
 * wired (Event binding) straight to UPlayerCharacterViewModel::ConfirmModal/CloseModal. None of that
 * wiring lives in C++: which name is valid, whether the action may run, what running it does is all
 * decided by UPlayerCharacterViewModel and URtsActions, not by this widget.
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

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;
};
