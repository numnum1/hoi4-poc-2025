#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OrganizationDetailWidget.generated.h"

class URtsHUDViewModel;

/**
 * PM_ViewOrganization's widget — same shape as UProvinceDetailWidget. Holds no data of its own:
 * what it shows (members, departments, balance) lives on a UOrganizationViewModel the Blueprint
 * creates itself as a CreateInstance view model, bound via a Destination Binding —
 * Initialize(FOrganizationId) <- the HUD view model's SelectedOrganizationId, the
 * same "Kind-ViewModels: Create Instance + Initialize" pattern UProvinceDetailWidget uses for
 * UProvinceViewModel. What this class pushes in (SetHUDViewModel) is only that source.
 */
UCLASS(Abstract)
class RTSUI_API UOrganizationDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by URtsHUDWidget, which owns the view model. */
	UFUNCTION(BlueprintCallable, Category="Organization Detail")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Organization Detail")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;
};
