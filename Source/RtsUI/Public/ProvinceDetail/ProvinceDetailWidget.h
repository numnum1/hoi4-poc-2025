#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProvinceDetailWidget.generated.h"

class UPlayerCharacterViewModel;

/**
 * PM_ViewProvince's widget — the C++ half of a widget whose Blueprint child carries only layout
 * and styling. Lives inside the played character's view for the same reason
 * UFoundOrganizationModalWidget does: it always belongs to the character currently looking.
 *
 * This widget holds no data of its own. What it shows (organizations present, building/character
 * counts) lives on a UProvinceViewModel, which the Blueprint creates itself as a CreateInstance
 * view model and binds via a Destination Binding: Initialize(FProvinceId) <- the played character
 * view model's SelectedProvinceId — the same "Kind-ViewModels: Create Instance + Initialize"
 * pattern UTimezoneViewModel uses. What this class pushes in (SetPlayerViewModel) is only that
 * source: a Property Path the Blueprint's destination binding reads from, exactly like
 * UFoundOrganizationModalWidget::SetPlayerViewModel.
 */
UCLASS(Abstract)
class RTSUI_API UProvinceDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Pushed in by the owning view once it has its view model. */
	UFUNCTION(BlueprintCallable, Category="Province Detail")
	void SetPlayerViewModel(UPlayerCharacterViewModel* InPlayerViewModel);

	UFUNCTION(BlueprintPure, Category="Province Detail")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;
};
