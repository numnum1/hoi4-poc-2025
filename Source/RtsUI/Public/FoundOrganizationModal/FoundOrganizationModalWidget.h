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
 * Cancel is wired (Event binding) straight to UPlayerCharacterViewModel::CloseModal; Confirm is wired
 * to UPlayerCharacterViewModel::HandleFoundOrganizationConfirmed, which receives everything the form
 * collected as event parameters and re-validates it through URtsActions before running anything —
 * the form's own validation is UX feedback, not the authority. The Blueprint should also call the
 * modal view model's ResetForm() whenever the modal becomes visible again, since the form does not
 * reset itself.
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
