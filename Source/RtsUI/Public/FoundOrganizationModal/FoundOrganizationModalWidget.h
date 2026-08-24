#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RtsIds.h"
#include "FoundOrganizationModalWidget.generated.h"

class UBuildingType;
class UFoundOrganizationModalWidgetViewModel;
class URtsHUDViewModel;

/**
 * The found-organization modal, as the C++ half of a widget whose Blueprint child carries only
 * layout and styling. Lives inside the played character's view because it is that character who
 * founds the organization — the modal has no meaning without one.
 *
 * The two view models it works with never reference each other. The form
 * (UFoundOrganizationModalWidgetViewModel) is a Create Instance entry the widget's own Blueprint
 * spawns and owns only the typed name; everything it displays is bound in the editor from
 * URtsHUDViewModel, which is where the build menu's pick, the map's site pick and the verdict on
 * all of it live. This class is what makes that source reachable: SetHUDViewModel is pushed down
 * from UPlayedCharacterView, and the Blueprint adds a "Property Path" view model entry pointing at
 * GetHUDViewModel to bind against.
 *
 * The name goes the other way through the same mechanism — a binding from the form's
 * OrganizationName onto URtsHUDViewModel::SetPendingOrganizationName — because the verdict cannot
 * be computed without it.
 *
 * Confirm/Cancel run through this class rather than through a dispatcher on the form: a
 * CreateInstance view model has no owner to report to, so it gets none (see "Callbacks gibt es nur
 * auf Manual-ViewModels" in CLAUDE.md — that rule is about ViewModels, not widgets, so it does not
 * forbid the dispatchers below). Confirm reads the name off the form and the building and site off
 * the HUD view model, then broadcasts OnConfirmed instead of calling UPlayerCharacterViewModel
 * directly — this widget does not need to know that class exists, only whoever owns it
 * (UPlayedCharacterView's Blueprint) does, and wires OnConfirmed/OnCancelled onto
 * HandleFoundOrganizationConfirmed/CloseModal. Those handlers re-validate through URtsActions before
 * running anything — the verdict shown here is UX feedback, not the authority. The Blueprint should
 * also call the form's ResetForm() whenever the modal becomes visible again, since the form does not
 * reset itself.
 */
UCLASS(Abstract)
class RTSUI_API UFoundOrganizationModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed down from UPlayedCharacterView. Kept as the Property Path source the form's own
	 * Blueprint bindings read from, and as what Confirm reads the collected building and site off. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Found Organization Modal")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

	/** Pushed in by the Blueprint once, with the form view model its own MVVM panel spawned. C++
	 * cannot reach that instance otherwise, and Confirm below has to read the name it collected. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void SetFormViewModel(UFoundOrganizationModalWidgetViewModel* InFormViewModel);

	UFUNCTION(BlueprintPure, Category="Found Organization Modal")
	UFoundOrganizationModalWidgetViewModel* GetFormViewModel() const { return FormViewModel; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFoundOrganizationConfirmed, FText, OrganizationName, UBuildingType*, HeadquartersType, FProvinceId, ProvinceId);

	/** Broadcast by Confirm() below with whatever was collected. Bind this (Event binding, via a
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

	/** Bind the confirm button's OnClicked to this. Reports what was collected and nothing else —
	 * this widget decides nothing about whether founding may run. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Confirm();

	/** Bind the cancel button's OnClicked to this. */
	UFUNCTION(BlueprintCallable, Category="Found Organization Modal")
	void Cancel();

private:
	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;

	UPROPERTY(Transient)
	TObjectPtr<UFoundOrganizationModalWidgetViewModel> FormViewModel;
};
