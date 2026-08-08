#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OrganizationDetailWidget.generated.h"

class UPlayerCharacterViewModel;

/**
 * PM_ViewOrganization's widget — same shape as UProvinceDetailWidget. Holds no data of its own:
 * what it shows (members, departments, balance) lives on a UOrganizationViewModel the Blueprint
 * creates itself as a CreateInstance view model, bound via a Destination Binding —
 * Initialize(FOrganizationId) <- the played character view model's SelectedOrganizationId, the
 * same "Kind-ViewModels: Create Instance + Initialize" pattern UProvinceDetailWidget uses for
 * UProvinceViewModel. What this class pushes in (SetPlayerViewModel) is only that source.
 */
UCLASS(Abstract)
class RTSUI_API UOrganizationDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by the owning view once it has its view model. */
	UFUNCTION(BlueprintCallable, Category="Organization Detail")
	void SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel);

	UFUNCTION(BlueprintPure, Category="Organization Detail")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;
};
