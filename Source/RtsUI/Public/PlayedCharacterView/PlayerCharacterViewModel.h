#pragma once

#include "CoreMinimal.h"
#include "Actions/RtsActionContext.h"
#include "Actions/RtsActionInput.h"
#include "Characters/CharacterViewModel.h"
#include "PlayedCharacterView/PlayerModal.h"
#include "Resources/ResourceType.h"
#include "Resources/RtsResourceContext.h"
#include "PlayerCharacterViewModel.generated.h"

class ARtsPlayer;
class UBuildingType;
class UResourceViewModel;
class USelectableCharacterViewModel;
class USelectableOrganizationViewModel;
class USelectablePositionViewModel;

/**
 * The played character, plus the widget state around it: which of the character's positions is
 * currently selected (or "self"), and the action list that goes with that selection.
 *
 * Everything UCharacterViewModel already exposes (name, portrait, abilities, positions, ...) is
 * inherited unchanged — this class adds only what the simulation has no notion of and the UI has
 * to invent itself. That is the line this class draws: a value derived from the simulation
 * belongs on the base class UCharacterViewModel, a value the UI made up (a selection, a
 * confirmation dialog's open/closed state, ...) belongs here.
 *
 * The picker the player selects with is SelectablePositions — one lean row per position, clicked
 * back up through HandlePositionClicked. Clicking the selected row clears the selection: the player
 * drops back to acting as themselves and no row is selected any more.
 *
 * The found-organization modal is the exception to "this class owns the widget state": its form
 * (typed name, headquarters pick, live validation) lives entirely in its own component,
 * UFoundOrganizationModalWidgetViewModel — a "smart form" CreateInstance view model the widget's
 * Blueprint spawns itself, never pushed into or read from here. This class only reports the one
 * thing that form cannot invent itself (SelectedProvinceId/SelectedProvinceName, picked on the world
 * map) and reacts once, in HandleFoundOrganizationConfirmed, to what the form collected. Note that
 * there is no organization type anywhere — an organization is a party because its headquarters is a
 * party office (a UBuildingType asset with bIsHeadquarters set), so picking the building *is*
 * picking what is being founded.
 *
 * The join- and recruit-organization modals are not "smart forms" the same way — they only ever
 * pick one row out of a list this class already builds (JoinableOrganizations/RecruitableCharacters),
 * so their pending selection and validation stay here (PendingJoinOrganizationId/MakeInput/
 * ConfirmModal), pushed onto their own modal widgets by one-way Function bindings same as before.
 *
 * The action list lives here rather than in a view model of its own precisely because it is a
 * function of that selection: the scope an action is offered for *is* SelectedPositionIndex, so a
 * separate object would only hold a second copy of it to keep in sync by hand. Everything that is
 * not the selection — which actions exist, whether one is allowed, what running it does — is a pure
 * function in URtsActions.
 *
 * Call InitializeForPlayer() once — UPlayedCharacterView does this in NativeOnInitialized, right
 * after constructing the instance — instead of the inherited Initialize(FCharacterId). It finds the
 * local ARtsPlayer, follows whichever character it currently controls, and re-initializes itself
 * automatically when the player switches characters.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UPlayerCharacterViewModel : public UCharacterViewModel
{
	GENERATED_BODY()

public:
	/** Finds the local player and starts following whichever character it controls. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel")
	void InitializeForPlayer();

	/** Act as the character themselves. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel")
	void SelectSelf();

	/** Act as one of the character's positions, indexed as GetPositions() does. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel")
	void SelectPosition(int32 InPositionIndex);

	//~ Modals -------------------------------------------------------------------------------------

	/** Opens a modal, resetting whatever the previous one had collected. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void OpenModal(EPlayerModal InModal);

	/** Closes the open modal without running anything. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void CloseModal();

	/** Which organization the join-organization modal is about to join. An invalid id means the
	 * player has not picked one yet, which is what the input gate refuses on. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void SelectJoinTarget(FOrganizationId InOrganizationId);

	/** Which character the recruit modal is about to recruit. An invalid id means the player has
	 * not picked one yet, which is what the input gate refuses on. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void SelectRecruitTarget(FCharacterId InCharacterId);

	/** Set from the world map click, and the only place a province is ever picked — see CLAUDE.md
	 * on the world map being *the* province interaction.
	 *
	 * What the click means depends on what is open: with the found-organization modal up it is the
	 * headquarters' site and the modal stays where it is, because opening PM_ViewProvince over it
	 * would tear down the half-filled form. Otherwise a valid id opens PM_ViewProvince and an
	 * invalid one closes it again, that modal being about nothing but this province. Either way the
	 * selection itself is one value, not one per reader. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void SelectProvince(FProvinceId InProvinceId);

	/** Runs the open modal's action after re-checking both gates, then closes it. Does nothing if
	 * either gate refuses — the widget's last pushed bCanConfirmModal is never trusted. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void ConfirmModal();

	/** Called by UFoundOrganizationModalWidget::Confirm — the found-organization modal is a "smart
	 * form" that collects and validates its own input (name, headquarters, province) and reports it
	 * here only once, instead of mirroring every keystroke and pick into this class the way the
	 * join/recruit modals still do. The values handed over are still not trusted: the gate runs again
	 * here exactly as ConfirmModal re-runs it for the other two modals. */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void HandleFoundOrganizationConfirmed(const FText& InOrganizationName, UBuildingType* InHeadquartersType, FProvinceId InProvinceId);

	/** Opens PM_ViewOrganization for InOrganizationId. Reached from two places, neither of them a C++
	 * selection change: a province screen row (through UProvinceDetailWidget::ViewOrganization) and
	 * the player's own position row — both mean "go look at this organization". */
	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Modal")
	void ViewOrganization(FOrganizationId InOrganizationId);

	//~ Actions — one entry point per action, since every action is known ahead of time, so a button
	//~ binds straight to the one it stands for rather than through a shared click-and-switch
	//~ indirection. Each re-derives the gate through that action's own URtsActions function and then
	//~ either runs it or opens its modal. -----------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Actions")
	void ClickFoundOrganization();

	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Actions")
	void ClickJoinOrganization();

	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Actions")
	void ClickLeaveOrganization();

	UFUNCTION(BlueprintCallable, Category="Player Character Viewmodel|Actions")
	void ClickRecruitCharacter();

protected:
	//~ Widget state — invented by the UI, not derived from the simulation ------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 SelectedPositionIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsActingAsSelf, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	bool bIsActingAsSelf = true;

	/** The picker row that is currently selected, or null while acting as self — the same state
	 * SelectedPositionIndex holds, as the object a widget can bind a highlight against. Written in
	 * exactly one place (ApplySelection) so the two can never disagree. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TObjectPtr<USelectablePositionViewModel> SelectedPosition;

	/** PM_None is a real state, so no separate "any modal open" flag exists to fall out of sync. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	EPlayerModal ActiveModal = EPlayerModal::PM_None;

	/** So a widget can drive its visibility without an enum comparison node. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsFoundOrganizationModalOpen, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bIsFoundOrganizationModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the join-organization modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsJoinOrganizationModalOpen, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bIsJoinOrganizationModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the view-province modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsViewProvinceModalOpen, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bIsViewProvinceModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the view-organization modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsViewOrganizationModalOpen, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bIsViewOrganizationModalOpen = false;

	/** Same idea as bIsFoundOrganizationModalOpen, for the recruit-character modal. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsRecruitCharacterModalOpen, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bIsRecruitCharacterModalOpen = false;

	/** The organization PM_ViewOrganization shows. Bound as a CreateInstance UOrganizationViewModel's
	 * Initialize(FOrganizationId) destination in Blueprint — same shape as SelectedProvinceId. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FOrganizationId SelectedOrganizationId;

	/** SelectedOrganizationId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	int32 SelectedOrganizationIdValue = INDEX_NONE;

	/** RtsHUDViewModel's copy is the source of truth (it is what a right click clears); this is that
	 * value pushed down into SelectProvince, kept here as a FieldNotify prop because widgets under
	 * this view model (ProvinceDetailWidget's CreateInstance UProvinceViewModel, the found-org
	 * modal's site field) only ever see PlayerCharacterViewModel, never the HUD view model
	 * directly — the same "data flows down" reason SelectedOrganizationId is stored here too. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FProvinceId SelectedProvinceId;

	/** SelectedProvinceId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	int32 SelectedProvinceIdValue = INDEX_NONE;

	/** The selected province's name, so the found-organization modal can show its site as a label
	 * without spawning a whole UProvinceViewModel for one string. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FText SelectedProvinceName;

	/** Which organization the join-organization modal will join, invalid until one is picked. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FOrganizationId PendingJoinOrganizationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	int32 PendingJoinOrganizationIdValue = INDEX_NONE;

	/** One row per organization present where the character currently stands, minus the ones it
	 * already belongs to. Rebuilt whenever the modal opens or the character moves/joins/leaves. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<USelectableOrganizationViewModel>> JoinableOrganizations;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	int32 JoinableOrganizationCount = 0;

	/** The picked row, or null while none is — same shape as SelectedPosition. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	TObjectPtr<USelectableOrganizationViewModel> SelectedJoinOrganization;

	/** Which character the recruit modal will recruit, invalid until one is picked. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FCharacterId PendingRecruitCharacterId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	int32 PendingRecruitCharacterIdValue = INDEX_NONE;

	/** One row per character standing where the acting position's organization is headquartered,
	 * minus the ones who already hold a position there. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<USelectableCharacterViewModel>> RecruitableCharacters;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	int32 RecruitableCharacterCount = 0;

	/** The picked row, or null while none is — same shape as SelectedJoinOrganization. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	TObjectPtr<USelectableCharacterViewModel> SelectedRecruitCharacter;

	/** Empty while the current input is usable. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	FText ModalErrorText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=CanConfirmModal, Category="Player Character Viewmodel|Modal", meta=(AllowPrivateAccess))
	bool bCanConfirmModal = false;

	//~ Derived from the selection above -----------------------------------------------------------

	/** One picker row per position the character holds, in the same order as GetPositions(). */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<USelectablePositionViewModel>> SelectablePositions;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 SelectablePositionCount = 0;

	//~ One fixed button per action — there is no per-action view model or list, see
	//~ ActionButtonWidget.h. Name and icon are constants the button's own Blueprint carries; only
	//~ visible/enabled/tooltip move with the current selection and are pushed from here. ------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsFoundOrganizationVisible, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsFoundOrganizationVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsFoundOrganizationEnabled, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsFoundOrganizationEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	FText FoundOrganizationTooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsJoinOrganizationVisible, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsJoinOrganizationVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsJoinOrganizationEnabled, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsJoinOrganizationEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	FText JoinOrganizationTooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsLeaveOrganizationVisible, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsLeaveOrganizationVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsLeaveOrganizationEnabled, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsLeaveOrganizationEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	FText LeaveOrganizationTooltip;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsRecruitCharacterVisible, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsRecruitCharacterVisible = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsRecruitCharacterEnabled, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	bool bIsRecruitCharacterEnabled = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel|Actions", meta=(AllowPrivateAccess))
	FText RecruitCharacterTooltip;

	/** One row per resource the current owner (the selection below) has — see MakeResourceContext.
	 * Same function of the selection as Actions, so it lives here rather than in a view model of
	 * its own for the same reason. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UResourceViewModel>> Resources;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Player Character Viewmodel", meta=(AllowPrivateAccess))
	int32 ResourceCount = 0;

public:
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
	bool IsActingAsSelf() const { return bIsActingAsSelf; }
	USelectablePositionViewModel* GetSelectedPosition() const { return SelectedPosition; }
	const TArray<TObjectPtr<USelectablePositionViewModel>>& GetSelectablePositions() const { return SelectablePositions; }
	int32 GetSelectablePositionCount() const { return SelectablePositionCount; }
	EPlayerModal GetActiveModal() const { return ActiveModal; }
	bool IsFoundOrganizationModalOpen() const { return bIsFoundOrganizationModalOpen; }
	bool IsJoinOrganizationModalOpen() const { return bIsJoinOrganizationModalOpen; }
	bool IsViewProvinceModalOpen() const { return bIsViewProvinceModalOpen; }
	bool IsViewOrganizationModalOpen() const { return bIsViewOrganizationModalOpen; }
	bool IsRecruitCharacterModalOpen() const { return bIsRecruitCharacterModalOpen; }
	FOrganizationId GetSelectedOrganizationId() const { return SelectedOrganizationId; }
	int32 GetSelectedOrganizationIdValue() const { return SelectedOrganizationIdValue; }
	FProvinceId GetSelectedProvinceId() const { return SelectedProvinceId; }
	int32 GetSelectedProvinceIdValue() const { return SelectedProvinceIdValue; }
	FText GetSelectedProvinceName() const { return SelectedProvinceName; }
	FOrganizationId GetPendingJoinOrganizationId() const { return PendingJoinOrganizationId; }
	int32 GetPendingJoinOrganizationIdValue() const { return PendingJoinOrganizationIdValue; }
	const TArray<TObjectPtr<USelectableOrganizationViewModel>>& GetJoinableOrganizations() const { return JoinableOrganizations; }
	int32 GetJoinableOrganizationCount() const { return JoinableOrganizationCount; }
	USelectableOrganizationViewModel* GetSelectedJoinOrganization() const { return SelectedJoinOrganization; }
	FCharacterId GetPendingRecruitCharacterId() const { return PendingRecruitCharacterId; }
	int32 GetPendingRecruitCharacterIdValue() const { return PendingRecruitCharacterIdValue; }
	const TArray<TObjectPtr<USelectableCharacterViewModel>>& GetRecruitableCharacters() const { return RecruitableCharacters; }
	int32 GetRecruitableCharacterCount() const { return RecruitableCharacterCount; }
	USelectableCharacterViewModel* GetSelectedRecruitCharacter() const { return SelectedRecruitCharacter; }
	FText GetModalErrorText() const { return ModalErrorText; }
	bool CanConfirmModal() const { return bCanConfirmModal; }
	bool IsFoundOrganizationVisible() const { return bIsFoundOrganizationVisible; }
	bool IsFoundOrganizationEnabled() const { return bIsFoundOrganizationEnabled; }
	FText GetFoundOrganizationTooltip() const { return FoundOrganizationTooltip; }
	bool IsJoinOrganizationVisible() const { return bIsJoinOrganizationVisible; }
	bool IsJoinOrganizationEnabled() const { return bIsJoinOrganizationEnabled; }
	FText GetJoinOrganizationTooltip() const { return JoinOrganizationTooltip; }
	bool IsLeaveOrganizationVisible() const { return bIsLeaveOrganizationVisible; }
	bool IsLeaveOrganizationEnabled() const { return bIsLeaveOrganizationEnabled; }
	FText GetLeaveOrganizationTooltip() const { return LeaveOrganizationTooltip; }
	bool IsRecruitCharacterVisible() const { return bIsRecruitCharacterVisible; }
	bool IsRecruitCharacterEnabled() const { return bIsRecruitCharacterEnabled; }
	FText GetRecruitCharacterTooltip() const { return RecruitCharacterTooltip; }
	const TArray<TObjectPtr<UResourceViewModel>>& GetResources() const { return Resources; }
	int32 GetResourceCount() const { return ResourceCount; }

protected:
	virtual void Refresh_Implementation() override;
	virtual void BeginDestroy() override;

private:
	/** The context the action catalogue is queried and gated with, built straight from the current
	 * selection. Never stored, so there is no second copy of the selection to keep in sync. */
	FRtsActionContext MakeContext() const;

	/** Same idea for the modal's collected values — built on demand, never stored twice. */
	FRtsActionInput MakeInput() const;

	/** The owner the resource list is asked about, built from the same selection MakeContext reads:
	 * the selected position's organization, or the played character itself while acting as self. */
	FRtsResourceContext MakeResourceContext() const;

	/** Opens InModal, or closes it again if it is the one already open — the toggle every action
	 * button that collects input before running shares. */
	void ToggleModal(EPlayerModal InModal);

	/** False for modals that only display something (PM_ViewProvince) rather than collect input for
	 * an action — those have nothing to confirm and must not run the confirm gate. */
	bool IsActionModal(EPlayerModal InModal) const;

	/** Both gates of whichever action the open modal is collecting for, empty while the input is
	 * usable. Empty too for a modal that confirms nothing, which IsActionModal separates out. */
	FText ComputeModalError() const;

	/** The one writer of SelectedPositionIndex/bIsActingAsSelf/SelectedPosition. INDEX_NONE means
	 * acting as self. Refreshes the action buttons, which are a function of exactly this. */
	void ApplySelection(int32 InPositionIndex);

	/** The one writer of PendingJoinOrganizationId/SelectedJoinOrganization, same reason
	 * ApplySelection is the one writer of the position selection. */
	void ApplyJoinTarget(FOrganizationId InOrganizationId);

	/** Rebuilds JoinableOrganizations: organizations present where the character stands, minus
	 * the ones it already belongs to. */
	void RefreshJoinableOrganizations();

	/** The one writer of PendingRecruitCharacterId/SelectedRecruitCharacter, same reason
	 * ApplyJoinTarget is the one writer of the join pick. */
	void ApplyRecruitTarget(FCharacterId InCharacterId);

	/** Rebuilds RecruitableCharacters: characters standing where the acting position's organization
	 * is headquartered, minus the ones who already hold a position there. */
	void RefreshRecruitableCharacters();

	void RefreshSelectablePositions();

	void RefreshActions();

	/** Rebuilds Resources for whatever MakeResourceContext currently returns. */
	void RefreshResources();

	/** Re-runs the input gate and pushes the result into ModalErrorText/bCanConfirmModal. */
	void RefreshModalValidation();

	/** Clicking the row that is already selected clears the selection instead of re-picking it. */
	UFUNCTION()
	void HandlePositionClicked(USelectablePositionViewModel* ClickedPosition);

	/** Clicking the row that is already the single expanded one collapses it instead of re-opening
	 * it — same shape as HandlePositionClicked. Re-runs RefreshResources so bIsExpanded is reapplied
	 * across every cached row, not just the clicked one. */
	UFUNCTION()
	void HandleResourceClicked(UResourceViewModel* ClickedResource);

	/** Clicking the picked row unpicks it, the same way HandlePositionClicked clears a selection. */
	UFUNCTION()
	void HandleJoinableOrganizationClicked(USelectableOrganizationViewModel* ClickedOrganization);

	/** Same toggle shape as HandleJoinableOrganizationClicked, for the recruit picker. */
	UFUNCTION()
	void HandleRecruitableCharacterClicked(USelectableCharacterViewModel* ClickedCharacter);

	/** UBanking::OnBalanceChanged was going unheard here — the money row only ever refreshed by
	 * accident, riding along on UCharacters::OnSimulated once an hour. Resolved/bound idempotently
	 * from Refresh_Implementation, the same shape Initialize resolves the base class's facades. */
	void BindBanking();

	UFUNCTION()
	void HandleBalanceChanged(FBankAccountId InAccountId);

	UFUNCTION()
	void HandleControlledCharacterChanged();

	UPROPERTY()
	TObjectPtr<ARtsPlayer> Player;

	/** One row per position index — a refresh that neither adds nor removes a position
	 * keeps the same row objects, so the picker does not churn and the bindings survive. */
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

	/** Resolved and bound once by BindBanking; unbound in BeginDestroy. */
	UPROPERTY()
	TObjectPtr<UBanking> Banking;

	/** Which resource is the single expanded one, and whether that is even the case right now —
	 * plain private state, not FieldNotify, because nothing binds to the id itself: only each row's
	 * own bIsExpanded (pushed by RefreshResources) is ever shown. Not folded into EResourceType as a
	 * "None" sentinel, because that enum is RtsResources' shared catalog, not UI state. */
	bool bHasExpandedResource = false;
	EResourceType ExpandedResourceType = EResourceType::RT_Money;
};
