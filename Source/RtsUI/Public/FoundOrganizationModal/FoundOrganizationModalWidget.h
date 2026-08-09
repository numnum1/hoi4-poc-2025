#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RtsIds.h"
#include "FoundOrganizationModalWidget.generated.h"

class UBuildingType;
class UFoundOrganizationModalWidgetViewModel;
class UPlayerCharacterViewModel;

/**
 * The found-organization modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Lives inside the played character's view because it is that character who
 * founds the organization — the modal has no meaning without one.
 *
 * This widget binds its own elements to its own component, UFoundOrganizationModalWidgetViewModel
 * — a CreateInstance view model the widget's Blueprint spawns itself in its View Models panel, not
 * something pushed in from C++. That component is a "smart form": name typed, headquarters picked,
 * the headquarters catalogue and whether the form is valid right now are all local state and
 * business logic it owns and computes itself, not values pushed down from here.
 *
 * What this class still pushes in (SetPlayerViewModel) is the played character's view model, kept
 * around only as a Property Path source for the handful of things the modal's own component cannot
 * know on its own: a one-way Function binding from UPlayerCharacterViewModel::CharacterId onto the
 * modal view model's Initialize (so it can resolve its own facades and funds), and one-way Function
 * bindings from GetSelectedProvinceId/GetSelectedProvinceName onto SetSelectedProvinceId/
 * SetSelectedProvinceName (the province is picked on the world map, not typed into this form).
 *
 * The confirm/cancel buttons run through this class rather than through a dispatcher on the form: a
 * CreateInstance view model has no owner to report to, so it gets none (see "Callbacks gibt es nur
 * auf Manual-ViewModels" in CLAUDE.md — that rule is about ViewModels, not widgets, so it does not
 * forbid the dispatchers below). The Blueprint calls SetFormViewModel once with the view model its
 * own MVVM panel spawned; Confirm/Cancel then read the form and broadcast OnConfirmed/OnCancelled
 * instead of calling UPlayerCharacterViewModel directly — this widget does not need to know that
 * class exists at all, only whoever owns it (UPlayedCharacterView's Blueprint) does, and wires those
 * two dispatchers onto UPlayerCharacterViewModel::HandleFoundOrganizationConfirmed/CloseModal. Those
 * handlers re-validate through URtsActions before running anything — the form's own validation is UX
 * feedback, not the authority. The Blueprint should also call the form's ResetForm() whenever the
 * modal becomes visible again, since the form does not reset itself.
 */
UCLASS(Abstract)
class RTSUI_API UFoundOrganizationModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by the owning view once it has its view model. Kept only as a Property Path source
	 * for the form's own Blueprint bindings (CharacterId, SelectedProvinceId/Name) — Confirm/Cancel
	 * below no longer call anything on it directly. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel);

	UFUNCTION(BlueprintPure, Category="Found Organization Modal")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

	/** Pushed in by the Blueprint once, with the form view model its own MVVM panel spawned. C++
	 * cannot reach that instance otherwise, and Confirm below has to read what it collected. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetFormViewModel(UFoundOrganizationModalWidgetViewModel* InFormViewModel);

	UFUNCTION(BlueprintPure, Category="Found Organization Modal")
	UFoundOrganizationModalWidgetViewModel* GetFormViewModel() const { return FormViewModel; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFoundOrganizationConfirmed, FText, OrganizationName, UBuildingType*, HeadquartersType, FProvinceId, ProvinceId);

	/** Broadcast by Confirm() below with whatever the form collected. Bind this (Event binding, via a
	 * Custom Event that forwards to a Call Function node — a dispatcher cannot be bound directly to a
	 * function on another object) to UPlayerCharacterViewModel::HandleFoundOrganizationConfirmed,
	 * which re-validates before running anything. */
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnFoundOrganizationConfirmed OnConfirmed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoundOrganizationCancelled);

	/** Broadcast by Cancel() below. Bind (through the same Custom-Event bridge as OnConfirmed) to
	 * UPlayerCharacterViewModel::CloseModal. */
	UPROPERTY(BlueprintAssignable, Category="Found Organization Modal")
	FOnFoundOrganizationCancelled OnCancelled;

	/** Bind the confirm button's OnClicked to this. Reads the form and broadcasts OnConfirmed with
	 * what it collected — this widget decides nothing about whether founding may run. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Confirm();

	/** Bind the cancel button's OnClicked to this. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Cancel();

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;

	UPROPERTY(Transient)
	TObjectPtr<UFoundOrganizationModalWidgetViewModel> FormViewModel;
};
