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

/**
 * The root widget for the character the player is currently playing. Owns the single
 * UPlayerCharacterViewModel every child widget under it reads from — the action list, the
 * position picker, the modals — and pushes it down to the ones that need it directly (currently
 * just the modal; see EPlayerModal for why a modal always belongs to this view).
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

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerCharacterViewModel> PlayerViewModel;

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
