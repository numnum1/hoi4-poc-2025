#pragma once

#include "CoreMinimal.h"
#include "Actions/RtsActionContext.h"
#include "Actions/RtsActionInput.h"
#include "MVVMViewModelBase.h"
#include "Resources/ResourceType.h"
#include "Resources/RtsResourceContext.h"
#include "RtsHUD/PlayerModal.h"
#include "RtsHUDViewModel.generated.h"

class ARtsMapPawn;
class ARtsPlayer;
class UBanking;
class UBuildings;
class UBuildingType;
class UCharacters;
class UCharacterViewModel;
class UDepartments;
class UOrganizationFocusAsset;
class UOrganizationFocuses;
class UOrganizations;
class UProvinces;
class UResourceViewModel;
class URoles;
class UTime;
class UUseOrganizationFocusTree;
class USelectableCharacterViewModel;
class USelectableOrganizationViewModel;
class USelectablePositionViewModel;
class UUseWorldMapView;
struct FPosition;

/**
 * Root view model for the HUD, owned by the HUD's root widget — every piece of state the UI itself
 * invents rather than derives from one domain: what the player picked on the map and in the build
 * menu, which of the played character's positions they are acting as, which modal is open and what
 * it has collected so far.
 *
 * All of it lives here rather than split across a second view model per screen, because it is all a
 * function of the same two things — the controlled character and the current selection. A second
 * object would only hold a copy of those to keep in sync by hand.
 *
 * The played character's own values (name, portrait, abilities, schedule, positions) are not
 * duplicated here: PlayedCharacter is a UCharacterViewModel this class owns and re-points whenever
 * ARtsPlayer switches character. What this class derives for itself it reads from the facades
 * instead, never through that child — both hang off the same UCharacters delegates, and the order
 * two handlers on one multicast run in is the order they were registered.
 *
 * MapView is the same arrangement for the world map: a UUseWorldMapView this class owns, holding
 * the active mode and computing the colors that follow from it. What this class pushes into it is
 * the founding overlay's two inputs, never a finished color map, so nothing here can drift out of
 * sync with what the map shows.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API URtsHUDViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	URtsHUDViewModel();

	/** Resolves the facades everything here reads, creates PlayedCharacter and MapView and starts
	 * following whichever character the local player controls. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void Initialize();

	/** Set from the world map click — see CLAUDE.md on the world map being *the* province
	 * interaction. An invalid id (also reached from a right click anywhere on the map, see
	 * HandleRightMouseButtonClicked) means nothing is selected.
	 *
	 * What the click means depends on what is open: with the found-organization modal up it is the
	 * headquarters' site and the modal stays where it is, because opening PM_ViewProvince over it
	 * would tear down the half-filled form. Otherwise a valid id opens PM_ViewProvince and an
	 * invalid one closes it again, that modal being about nothing but this province. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetSelectedProvinceId(FProvinceId InSelectedProvinceId);

#pragma region Position selection
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Positions")
	void SelectSelf();

	/** Act as one of the character's positions, indexed as SelectablePositions is. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Positions")
	void SelectPosition(int32 InPositionIndex);
#pragma endregion

#pragma region Modals
	/** Opens a modal, resetting whatever the previous one had collected. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Modal")
	void OpenModal(EPlayerModal InModal);

	/** Closes the open modal without running anything. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Modal")
	void CloseModal();

	/** Which organization the join-organization modal is about to join. An invalid id means the
	 * player has not picked one yet, which is what the input gate refuses on. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Modal")
	void SelectJoinTarget(FOrganizationId InOrganizationId);

	/** Which character the recruit modal is about to recruit. An invalid id means the player has
	 * not picked one yet, which is what the input gate refuses on. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Modal")
	void SelectRecruitTarget(FCharacterId InCharacterId);

	/** Runs the open modal's action after re-checking both gates, then closes it. Does nothing if
	 * either gate refuses — the widget's last pushed bCanConfirmModal is never trusted. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Modal")
	void ConfirmModal();

	/** Called by UFoundOrganizationModalWidget::Confirm — the found-organization modal is a "smart
	 * form" that collects and validates its own input (name, headquarters, province) and reports it
	 * here only once, instead of mirroring every keystroke and pick into this class the way the
	 * join/recruit modals still do. The values handed over are still not trusted: the gate runs again
	 * here exactly as ConfirmModal re-runs it for the other two modals. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Modal")
	void HandleFoundOrganizationConfirmed(const FText& InOrganizationName, UBuildingType* InHeadquartersType, FProvinceId InProvinceId);

	/** Opens PM_ViewOrganization for InOrganizationId. Reached from two places, neither of them a C++
	 * selection change: a province screen row (through UProvinceDetailWidget::ViewOrganization) and
	 * the player's own position row — both mean "go look at this organization". */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Modal")
	void ViewOrganization(FOrganizationId InOrganizationId);
#pragma endregion

#pragma region Actions
	//~ One entry point per action, since every action is known ahead of time, so a button binds
	//~ straight to the one it stands for rather than through a shared click-and-switch indirection.
	//~ Each re-derives the gate through that action's own URtsActions function and then either runs
	//~ it or opens its modal.

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Actions")
	void ClickFoundOrganization();

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Actions")
	void ClickJoinOrganization();

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Actions")
	void ClickLeaveOrganization();

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Actions")
	void ClickRecruitCharacter();
#pragma endregion

#pragma region Focuses
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Focuses")
	void StartFocus(UOrganizationFocusAsset* InFocus);

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel|Focuses")
	void ChooseFocusEventOption(int32 InOptionIndex);
#pragma endregion

#pragma region Build Menu
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	bool ComputeIsBuildMenuOpen() const;

	/** What the player has picked in the build menu, or null. The authority on the pick — the row's
	 * own bIsSelected is display only, the same split UBuildMenuEntryViewModel documents. Read by
	 * whichever process is collecting a building, and pushed into MapView as the founding overlay's
	 * subject. */
	UBuildingType* GetSelectedBuildingType() const { return SelectedBuildingType; }

	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	TArray<UBuildingType*> ComputeBuildings() const;
#pragma endregion

#pragma region Found Organization
	/**
	 * The name the founding form has collected so far, mirrored up by a binding from
	 * UFoundOrganizationModalWidgetViewModel::OrganizationName.
	 *
	 * It lives here because judging it needs the acting character, their money, the site province
	 * and the nation that owns it — none of which the form can resolve. The form stays the field's
	 * owner; this is the copy the verdict below is computed from.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetPendingOrganizationName(const FText& InPendingOrganizationName);

	/** Why founding would be refused right now, or empty. Computed rather than stored, the same
	 * reason the map colors are: nothing can push a verdict in and none of its inputs can drift out
	 * of sync with it. Still only UX feedback — HandleFoundOrganizationConfirmed re-runs both gates
	 * on confirm, which is what actually decides. */
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	FText ComputeFoundOrganizationError() const;

	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	bool CanConfirmFoundOrganization() const;
#pragma endregion

private:
	/** Mirrors ARtsPlayer::ControlledCharacterId — pulled in BindPlayer and kept current by
	 * HandleControlledCharacterChanged, never pushed in from a widget. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FCharacterId ControlledCharacterId;

	/** The played character as a plain read-only view model, re-pointed at ControlledCharacterId
	 * whenever the player switches. Everything a widget wants to *show* about them binds through
	 * here; nothing in this class reads its values back. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	TObjectPtr<UCharacterViewModel> PlayedCharacter;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	TObjectPtr<UUseWorldMapView> MapView;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	TObjectPtr<UUseOrganizationFocusTree> FocusTree;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FProvinceId SelectedProvinceId;

	/** SelectedProvinceId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	int32 SelectedProvinceIdValue = INDEX_NONE;

	/** The selected province's name, so a form showing the picked site does not need UProvinces of
	 * its own. Derived in SetSelectedProvinceId, never pushed. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FText SelectedProvinceName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	int32 SelectedBuildingCost = 0;

	/** Same reason as SelectedProvinceName: a form showing what is about to be built binds this
	 * rather than reaching for the asset. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FText SelectedBuildingName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FText PendingOrganizationName;

	//~ Position selection --------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Positions", meta = (AllowPrivateAccess))
	int32 SelectedPositionIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Positions", meta = (AllowPrivateAccess))
	bool bIsActingAsSelf = true;

	/** The picker row that is currently selected, or null while acting as self — the same state
	 * SelectedPositionIndex holds, as the object a widget can bind a highlight against. Written in
	 * exactly one place (ApplySelection) so the two can never disagree. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Positions", meta = (AllowPrivateAccess))
	TObjectPtr<USelectablePositionViewModel> SelectedPosition;

	/** One picker row per position the character holds, in the order the facade reports them. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Positions", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<USelectablePositionViewModel>> SelectablePositions;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Positions", meta = (AllowPrivateAccess))
	int32 SelectablePositionCount = 0;

	//~ Modals -------------------------------------------------------------------------------------

	/** PM_None is a real state, so no separate "any modal open" flag exists to fall out of sync. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	EPlayerModal ActiveModal = EPlayerModal::PM_None;

	/** So a widget can drive its visibility without an enum comparison node. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	bool bIsFoundOrganizationModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the join-organization modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	bool bIsJoinOrganizationModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the view-province modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	bool bIsViewProvinceModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the view-organization modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	bool bIsViewOrganizationModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the recruit-character modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	bool bIsRecruitCharacterModalOpen = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	bool bIsFocusTreeModalOpen = false;

	/** The organization PM_ViewOrganization shows. Bound as a CreateInstance UOrganizationViewModel's
	 * Initialize(FOrganizationId) destination in Blueprint — same shape as SelectedProvinceId. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	FOrganizationId SelectedOrganizationId;

	/** SelectedOrganizationId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	int32 SelectedOrganizationIdValue = INDEX_NONE;

	/** Which organization the join-organization modal will join, invalid until one is picked. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	FOrganizationId PendingJoinOrganizationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	int32 PendingJoinOrganizationIdValue = INDEX_NONE;

	/** One row per organization present where the character currently stands, minus the ones it
	 * already belongs to. Rebuilt whenever the modal opens or the character moves/joins/leaves. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<USelectableOrganizationViewModel>> JoinableOrganizations;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	int32 JoinableOrganizationCount = 0;

	/** The picked row, or null while none is — same shape as SelectedPosition. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	TObjectPtr<USelectableOrganizationViewModel> SelectedJoinOrganization;

	/** Which character the recruit modal will recruit, invalid until one is picked. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	FCharacterId PendingRecruitCharacterId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	int32 PendingRecruitCharacterIdValue = INDEX_NONE;

	/** One row per character standing where the acting position's organization is headquartered,
	 * minus the ones who already hold a position there. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<USelectableCharacterViewModel>> RecruitableCharacters;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	int32 RecruitableCharacterCount = 0;

	/** The picked row, or null while none is — same shape as SelectedJoinOrganization. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	TObjectPtr<USelectableCharacterViewModel> SelectedRecruitCharacter;

	/** Empty while the current input is usable. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	FText ModalErrorText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Modal", meta = (AllowPrivateAccess))
	bool bCanConfirmModal = false;

	//~ Actions — one fixed button per action, there is no per-action view model or list, see
	//~ ActionButtonWidget.h. Name and icon are constants the button's own Blueprint carries; only
	//~ visible/enabled/tooltip move with the current selection and are pushed from here. -----------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsFoundOrganizationVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsFoundOrganizationEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	FText FoundOrganizationTooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsJoinOrganizationVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsJoinOrganizationEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	FText JoinOrganizationTooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsLeaveOrganizationVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsLeaveOrganizationEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	FText LeaveOrganizationTooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsRecruitCharacterVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	bool bIsRecruitCharacterEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel|Actions", meta = (AllowPrivateAccess))
	FText RecruitCharacterTooltip;

	/** One row per resource the current owner has — see MakeResourceContext. Same function of the
	 * position selection the action buttons are. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<UResourceViewModel>> Resources;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	int32 ResourceCount = 0;

public:
	FProvinceId GetSelectedProvinceId() const { return SelectedProvinceId; }

	UUseWorldMapView* GetMapView() const { return MapView; }

	UUseOrganizationFocusTree* GetFocusTree() const { return FocusTree; }

private:
	//~ Build menu ---------------------------------------------------------------------------------

	/** The one writer of SelectedBuildingType, and through the rebuild below of the row highlight
	 * and SelectedBuildingCost derived from it — so the pick and what is shown cannot disagree. */
	void ApplySelectedBuildingType(UBuildingType* InBuildingType);

	void RefreshBuildMenu();

	void RefreshMapOverlay();

	/** Whether the menu currently offers headquarters only. Derived from ActiveModal rather than
	 * pushed in when the menu is opened: the founding process is the only thing that narrows the
	 * catalogue, and deriving it means closing that modal cannot leave the filter behind. */
	bool IsBuildMenuHeadquartersOnly() const;

	UFUNCTION()
	void HandleBalanceChanged(FBankAccountId InAccountId);

	//~ Position selection --------------------------------------------------------------------------

	/** The one writer of SelectedPositionIndex/bIsActingAsSelf/SelectedPosition. INDEX_NONE means
	 * acting as self. Refreshes the action buttons, which are a function of exactly this. */
	void ApplySelection(int32 InPositionIndex);

	void RefreshSelectablePositions();

	/** Clicking the row that is already selected clears the selection instead of re-picking it. */
	UFUNCTION()
	void HandlePositionClicked(USelectablePositionViewModel* ClickedPosition);

	/** Everything that follows from the played character or their positions having changed: the
	 * picker, the selection recovered by organization rather than by index, and the open modal's
	 * verdict. */
	void RefreshPlayedCharacter();

	UFUNCTION()
	void HandlePlayedCharacterEvent(FCharacterId InCharacterId);

	//~ Modals --------------------------------------------------------------------------------------

	/** The one writer of ActiveModal and of everything derived from it: the five open flags, the
	 * join and recruit picks the previous modal collected, the build menu's filter and pick, the
	 * typed organization name, and the map's colors — a modal may paint the map for its own purposes
	 * and repainting here is what puts it back, so no modal has to undo its own colors. */
	void ApplyModal(EPlayerModal InModal);

	/** Opens InModal, or closes it again if it is the one already open — the toggle every action
	 * button that collects input before running shares. */
	void ToggleModal(EPlayerModal InModal);

	/** False for modals that only display something (PM_ViewProvince) rather than collect input for
	 * an action — those have nothing to confirm and must not run the confirm gate. */
	bool IsActionModal(EPlayerModal InModal) const;

	/** Both gates of whichever action the open modal is collecting for, empty while the input is
	 * usable. Empty too for a modal that confirms nothing, which IsActionModal separates out. */
	FText ComputeModalError() const;

	/** Re-runs the input gate and pushes the result into ModalErrorText/bCanConfirmModal. */
	void RefreshModalValidation();

	/** The one writer of PendingJoinOrganizationId/SelectedJoinOrganization, same reason
	 * ApplySelection is the one writer of the position selection. */
	void ApplyJoinTarget(FOrganizationId InOrganizationId);

	/** Rebuilds JoinableOrganizations: organizations present where the character stands, minus
	 * the ones it already belongs to. */
	void RefreshJoinableOrganizations();

	/** Clicking the picked row unpicks it, the same way HandlePositionClicked clears a selection. */
	UFUNCTION()
	void HandleJoinableOrganizationClicked(USelectableOrganizationViewModel* ClickedOrganization);

	/** The one writer of PendingRecruitCharacterId/SelectedRecruitCharacter, same reason
	 * ApplyJoinTarget is the one writer of the join pick. */
	void ApplyRecruitTarget(FCharacterId InCharacterId);

	/** Rebuilds RecruitableCharacters: characters standing where the acting position's organization
	 * is headquartered, minus the ones who already hold a position there. */
	void RefreshRecruitableCharacters();

	/** Same toggle shape as HandleJoinableOrganizationClicked, for the recruit picker. */
	UFUNCTION()
	void HandleRecruitableCharacterClicked(USelectableCharacterViewModel* ClickedCharacter);

	//~ Actions and resources -----------------------------------------------------------------------

	/** The context the action catalogue is queried and gated with, built straight from the current
	 * selection. Never stored, so there is no second copy of the selection to keep in sync. */
	FRtsActionContext MakeContext() const;

	/** Same idea for the join/recruit modals' collected values — built on demand, never stored
	 * twice. */
	FRtsActionInput MakeInput() const;

	void RefreshActions();

	void RefreshFocusTree();

	/** The owner the resource list is asked about, built from the same selection MakeContext reads:
	 * the selected position's organization, or the played character itself while acting as self. */
	FRtsResourceContext MakeResourceContext() const;

	/** Rebuilds Resources for whatever MakeResourceContext currently returns. */
	void RefreshResources();

	/** Clicking the row that is already the single expanded one collapses it instead of re-opening
	 * it — same shape as HandlePositionClicked. Re-runs RefreshResources so bIsExpanded is reapplied
	 * across every cached row, not just the clicked one. */
	UFUNCTION()
	void HandleResourceClicked(UResourceViewModel* ClickedResource);

	/** The position the current selection points at, or null while acting as self — the one lookup
	 * every consumer of the selection goes through, so none of them can disagree about which
	 * position is meant. */
	const FPosition* FindSelectedPosition(const TArray<FPosition>& InPositions) const;

	//~ Found organization -------------------------------------------------------------------------

	/** MakeContext forced to RAS_Self: founding is a self action, and its verdict has to stay about
	 * founding even while a position happens to be selected. */
	FRtsActionContext MakeFoundOrganizationContext() const;

	/** The collected values, in the shape the gate takes them. */
	FRtsActionInput MakeFoundOrganizationInput() const;

	/** Re-announces the verdict. Called wherever one of its inputs moves — there is no single
	 * setter to hang it off, since the name, the building and the site all arrive separately. */
	void BroadcastFoundOrganizationVerdict();

	void BindFacades();
	void UnbindFacades();

	/** Resolves the local ARtsPlayer, binds HandleControlledCharacterChanged to it and pulls the
	 * current controlled character in — called from Initialize(), so neither the id nor the
	 * right-click binding depends on Blueprint ever pushing anything in. Idempotent, safe to call
	 * again. */
	void BindPlayer();

	/** Re-run whenever the controlled character changes, since the pawn can only be resolved once
	 * the player controller actually possesses one, and idempotent for the same reason. */
	void BindPawnRightClick();

	UFUNCTION()
	void HandleControlledCharacterChanged();

	/** Bound to the possessed pawn's OnRightMouseButtonClicked — a right click anywhere on the map
	 * clears the province selection the same way an invalid pick from the map itself would. */
	UFUNCTION()
	void HandleRightMouseButtonClicked();

	UPROPERTY(Transient)
	TObjectPtr<UProvinces> Provinces;

	UPROPERTY(Transient)
	TObjectPtr<UOrganizations> Organizations;

	UPROPERTY(Transient)
	TObjectPtr<UDepartments> Departments;

	UPROPERTY(Transient)
	TObjectPtr<UBuildings> Buildings;

	UPROPERTY(Transient)
	TObjectPtr<UBanking> Banking;

	UPROPERTY(Transient)
	TObjectPtr<UCharacters> Characters;

	UPROPERTY(Transient)
	TObjectPtr<URoles> Roles;

	UPROPERTY(Transient)
	TObjectPtr<UOrganizationFocuses> OrganizationFocuses;

	UPROPERTY(Transient)
	TObjectPtr<UTime> Time;

	UPROPERTY()
	TObjectPtr<ARtsPlayer> Player;

	/** The pawn OnRightMouseButtonClicked is bound to, tracked separately from Player because the
	 * pawn can change (or be unpossessed) without the controlled character changing. */
	UPROPERTY()
	TObjectPtr<ARtsMapPawn> Pawn;

	/** The pick itself. A plain UPROPERTY rather than FieldNotify: nothing binds to the asset, only
	 * to SelectedBuildingName and SelectedBuildingCost derived from it. */
	UPROPERTY()
	TObjectPtr<UBuildingType> SelectedBuildingType;

	/** One row per position index — a refresh that neither adds nor removes a position keeps the
	 * same row objects, so the picker does not churn and the bindings survive. */
	UPROPERTY()
	TArray<TObjectPtr<USelectablePositionViewModel>> SelectablePositionCache;

	/** Keyed by EResourceType — a rebuild that offers the same resources keeps the same row
	 * objects. */
	UPROPERTY()
	TMap<EResourceType, TObjectPtr<UResourceViewModel>> ResourceEntryCache;

	/** Same idea, keyed by the organization id. */
	UPROPERTY()
	TMap<FOrganizationId, TObjectPtr<USelectableOrganizationViewModel>> JoinableOrganizationCache;

	/** Same idea, keyed by the character id. */
	UPROPERTY()
	TMap<FCharacterId, TObjectPtr<USelectableCharacterViewModel>> RecruitableCharacterCache;

	/** Which resource is the single expanded one, and whether that is even the case right now —
	 * plain private state, not FieldNotify, because nothing binds to the id itself: only each row's
	 * own bIsExpanded (pushed by RefreshResources) is ever shown. Not folded into EResourceType as a
	 * "None" sentinel, because that enum is RtsResources' shared catalog, not UI state. */
	bool bHasExpandedResource = false;
	EResourceType ExpandedResourceType = EResourceType::RT_Money;
};
