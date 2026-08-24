#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayedCharacterView.generated.h"

class UFoundOrganizationModalWidget;
class UJoinOrganizationModalWidget;
class UOrganizationDetailWidget;
class UPlayerCharacterViewModel;
class UProvinceDetailWidget;
class URecruitCharacterModalWidget;
class URtsHUDViewModel;

/**
 * The root widget for the character the player is currently playing. Owns the single
 * UPlayerCharacterViewModel every child widget under it reads from — the action list, the
 * position picker, the modals — and pushes it down to the ones that need it directly (see
 * EPlayerModal for why a modal always belongs to this view).
 *
 * It also relays URtsHUDViewModel downward without owning it; see SetHUDViewModel.
 *
 * Creates and initializes its view model itself in NativeOnInitialized rather than leaving that to
 * the Blueprint's MVVM panel, per "Widgets sind C++-Klassen" in CLAUDE.md: which object exists and
 * when is a decision, not layout. The Blueprint binds its own FieldNotify widgets to a view model
 * whose Creation Type is Property Path → GetPlayerViewModel, not "Create Instance" — that setting
 * would spawn a second, un-initialized UPlayerCharacterViewModel alongside this one.
 */
UCLASS(Abstract)
class RTSUI_API UPlayedCharacterView : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Played Character View")
	UPlayerCharacterViewModel* GetPlayerViewModel() const { return PlayerViewModel; }

	/**
	 * Pushed down from URtsHUDWidget, which owns it, and forwarded to the children that bind
	 * against it. Not created here: the HUD view model outlives and outranks this view — it holds
	 * the build menu, the map selection and the map coloring, none of which belong to one character.
	 *
	 * Pushed rather than resolved, and forwarded immediately rather than in NativeOnInitialized,
	 * because a child widget is initialized before its parent: by the time the HUD hands this down,
	 * this view's own NativeOnInitialized has already run.
	 */
	UFUNCTION(BlueprintCallable, Category="Played Character View")
	void SetHUDViewModel(URtsHUDViewModel* InHUDViewModel);

	UFUNCTION(BlueprintPure, Category="Played Character View")
	URtsHUDViewModel* GetHUDViewModel() const { return HUDViewModel; }

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;

	UPROPERTY(Transient)
	TObjectPtr<URtsHUDViewModel> HUDViewModel;

	/** Optional so this class still initializes cleanly before the Blueprint has placed the modal
	 * (or for a derived view that never needs it). Pushed to as soon as both exist. */
	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetOptional, AllowPrivateAccess))
	TObjectPtr<UFoundOrganizationModalWidget> FoundOrganizationModal;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetOptional, AllowPrivateAccess))
	TObjectPtr<UJoinOrganizationModalWidget> JoinOrganizationModal;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetOptional, AllowPrivateAccess))
	TObjectPtr<UProvinceDetailWidget> ProvinceDetail;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetOptional, AllowPrivateAccess))
	TObjectPtr<UOrganizationDetailWidget> OrganizationDetail;

	/** Same optionality reasoning as FoundOrganizationModal. */
	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetOptional, AllowPrivateAccess))
	TObjectPtr<URecruitCharacterModalWidget> RecruitCharacterModal;
};
