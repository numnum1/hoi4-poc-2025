#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RtsIds.h"
#include "ProvinceDetailWidget.generated.h"

class URtsHUDViewModel;

/**
 * PM_ViewProvince's widget — the C++ half of a widget whose Blueprint child carries only layout
 * and styling. Lives under the HUD for the same reason
 * UFoundOrganizationModalWidget does: it always belongs to the character currently looking.
 *
 * This widget holds no data of its own. What it shows (organizations present, building/character
 * counts) lives on a UProvinceViewModel, which the Blueprint creates itself as a CreateInstance
 * view model and binds via a Destination Binding: Initialize(FProvinceId) <- the HUD view
 * model's SelectedProvinceId — the same "Kind-ViewModels: Create Instance + Initialize"
 * pattern UTimezoneViewModel uses. What this class pushes in (SetHUDViewModel) is only that
 * source: a Property Path the Blueprint's destination binding reads from, exactly like
 * UJoinOrganizationModalWidget::SetHUDViewModel.
 *
 * It is also the way a click gets *out* of that view model. UProvinceViewModel is a pure
 * derivation of the simulation and may not know URtsHUDViewModel, and being CreateInstance it
 * has no dispatcher to report through either (see "Callbacks gibt es nur auf Manual-ViewModels" in
 * CLAUDE.md). This class holds both sides, so an organization row's Blueprint calls
 * ViewOrganization() below and the decision stays in C++.
 */
UCLASS(Abstract)
class RTSUI_API UProvinceDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by URtsHUDWidget, which owns the view model. */
	UFUNCTION(BlueprintCallable, Category="Province Detail")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Province Detail")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

	/** Bind an organization row's click to this, passing that row's OrganizationId. Opens the
	 * organization screen for it — the HUD view model owns that state and decides. */
	UFUNCTION(BlueprintCallable, Category="Province Detail")
	void ViewOrganization(FOrganizationId InOrganizationId);

private:
	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;
};
