#pragma once

#include "CoreMinimal.h"
#include "Actions/RtsActionContext.h"
#include "Actions/RtsActionInput.h"
#include "MapModes/MapModeType.h"
#include "MapTypes.h"
#include "MVVMViewModelBase.h"
#include "PlayedCharacterView/PlayerCharacterViewModel.h"
#include "ProvinceVisuals.h"
#include "MapTooltip/MapTooltipContent.h"
#include "RtsHUDViewModel.generated.h"

class ARtsMapPawn;
class ARtsPlayer;
class UBanking;
class UBuildings;
class UBuildingType;
class UCharacters;
class UDepartments;
class UMapModeEntryViewModel;
class UNations;
class UOrganizations;
class UPopulations;
class UProvinces;
class URoles;

/**
 * Root view model for the HUD, owned by the HUD's root widget the same way UPlayedCharacterView
 * owns UPlayerCharacterViewModel — widget state the HUD itself invents, not derived from any one
 * domain.
 *
 * It is also where the world map gets its colors. Which map mode is showing is HUD state, and the
 * colors that follow from it are a pure function of that mode plus the facades, so ProvinceVisuals
 * is computed rather than stored: nothing can push a color in, and nothing can drift out of sync.
 * The map itself derives nothing — it is handed the finished result through UWorldMapWidget.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API URtsHUDViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	URtsHUDViewModel();

	/** Resolves the facades the map modes read and builds the mode picker. Until this runs,
	 * ProvinceVisuals is empty. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetActiveModal(EPlayerModal InActiveModal);

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetSelectedPositionIndex(int32 InSelectedPositionIndex);

	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetActiveMapMode(EMapMode InActiveMapMode);

	/** Set from the world map click — see CLAUDE.md on the world map being *the* province
	 * interaction. An invalid id (also reached from a right click anywhere on the map, see
	 * HandleRightMouseButtonClicked) means nothing is selected. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetSelectedProvinceId(FProvinceId InSelectedProvinceId);

	/** Set from UWorldMapWidget::OnHoveredProvinceIdChanged. Purely for what the HUD wants to say
	 * about the province under the cursor - the map draws its own highlight and needs nothing from
	 * here. An invalid id means the cursor is on no province. */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetHoveredProvinceId(FProvinceId InHoveredProvinceId);

	/** The color of every living province under the active map mode. Computed on demand — see the
	 * class comment for why this is not a stored property. */
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	TMap<FProvinceId, FProvinceVisuals> GetProvinceVisuals() const;

	/** Every living nation's name and the provinces it holds, for the map's country labels. Computed
	 * on demand for the same reason ProvinceVisuals is — see the class comment. */
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	TMap<FNationId, FWorldMapNationData> GetNationData() const;

#pragma region Build Menu
	/**
	 * Opens or closes the one build menu the HUD has. There is exactly one — a process that needs a
	 * building picked (founding an organization today) brings this one up instead of carrying a
	 * picker of its own, which is why the state lives here and not on the process' modal.
	 *
	 * Closing it drops the pick: a menu that comes back up still showing yesterday's selection would
	 * be a second source of truth for whatever process opens it next.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rts HUD ViewModel")
	void SetBuildMenuOpen(bool bInOpen);

	/** What the player has picked in the build menu, or null. The authority on the pick — the row's
	 * own bIsSelected is display only, the same split UBuildMenuEntry documents. Read by
	 * whichever process is collecting a building, and by ComputeTooltip so hovering a province can
	 * say something about the building about to be placed there. */
	UBuildingType* GetSelectedBuildingType() const { return SelectedBuildingType; }
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
	 * reason ProvinceVisuals is: nothing can push a verdict in and none of its inputs can drift out
	 * of sync with it. Still only UX feedback — UPlayerCharacterViewModel re-runs both gates on
	 * confirm, which is what actually decides. */
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	FText ComputeFoundOrganizationError() const;

	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	bool CanConfirmFoundOrganization() const;
#pragma endregion

#pragma region Build menu
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	TArray<UBuildingType*> ComputeBuildings() const;
#pragma endregion

#pragma region Tooltip
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	FMapTooltipContent ComputeTooltip() const;
#pragma endregion 
	
	virtual void BeginDestroy() override;

private:
	/** Mirrors ARtsPlayer::ControlledCharacterId — pulled in BindPlayer and kept current by
	 * HandleControlledCharacterChanged, never pushed in from a widget. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FCharacterId ControlledCharacterId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	EPlayerModal ActiveModal;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	int32 SelectedPositionIndex;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	EMapMode ActiveMapMode = EMapMode::EMM_Political;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FProvinceId SelectedProvinceId;

	/** SelectedProvinceId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	int32 SelectedProvinceIdValue = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FProvinceId HoveredProvinceId;

	/** The selected province's name, so a form showing the picked site does not need UProvinces of
	 * its own. Derived in SetSelectedProvinceId, never pushed. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FText SelectedProvinceName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsBuildMenuOpen, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	bool bIsBuildMenuOpen = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	int32 SelectedBuildingCost = 0;

	/** Same reason as SelectedProvinceName: a form showing what is about to be built binds this
	 * rather than reaching for the asset. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FText SelectedBuildingName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	FText PendingOrganizationName;

	/** One row per mode, cached rather than rebuilt: the set of modes is fixed, only which one is
	 * active changes. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<UMapModeEntryViewModel>> MapModes;

	UFUNCTION()
	void HandleMapModeClicked(UMapModeEntryViewModel* InClickedEntry);

	//~ Build menu ---------------------------------------------------------------------------------

	/** The one writer of SelectedBuildingType, and through the rebuild below of the row highlight
	 * and SelectedBuildingCost derived from it — so the pick and what is shown cannot disagree. */
	void ApplySelectedBuildingType(UBuildingType* InBuildingType);

	void RefreshBuildMenu();

	/** Whether the menu currently offers headquarters only. Derived from ActiveModal rather than
	 * pushed in when the menu is opened: the founding process is the only thing that narrows the
	 * catalogue, and deriving it means closing that modal cannot leave the filter behind. */
	bool IsBuildMenuHeadquartersOnly() const;


	UFUNCTION()
	void HandleBalanceChanged(FBankAccountId InAccountId);

	//~ Found organization -------------------------------------------------------------------------

	/** Everything URtsActions needs to judge founding, assembled from the facades resolved here —
	 * URtsActions never resolves a subsystem itself, see NationActions.h for the same shape. */
	FRtsActionContext MakeFoundOrganizationContext() const;

	/** The collected values, in the shape the gate takes them. */
	FRtsActionInput MakeFoundOrganizationInput() const;

	/** Re-announces the verdict. Called wherever one of its inputs moves — there is no single
	 * setter to hang it off, since the name, the building and the site all arrive separately. */
	void BroadcastFoundOrganizationVerdict();

	/** The acting character's own nation: whoever owns the province they are standing in, the same
	 * derivation UCharacterViewModel::CurrentNationId makes for display. */
	FNationId GetActingNationId() const;

	/** Every living province colored by founding eligibility for the picked building — grey for
	 * another nation's territory, red where founding is refused for another reason, blue where the
	 * character already has one, green where a site is free.
	 *
	 * Returned from GetProvinceVisuals instead of painted onto the map directly, so the founding
	 * overlay is a derivation like every other map coloring here and nothing has to undo it: the
	 * moment the modal closes or the pick is dropped, the active map mode is what comes back. */
	TMap<FProvinceId, FProvinceVisuals> BuildFoundingSiteVisuals() const;

	//~ Anything that can move a province's color under the active mode ---------------------------

	UFUNCTION()
	void HandleProvincesSetup();

	UFUNCTION()
	void HandleProvinceChanged(FProvinceId InProvinceId);

	UFUNCTION()
	void HandleOrganizationsChanged();

	UFUNCTION()
	void HandleOrganizationChanged(FOrganizationId InOrganizationId);

	UFUNCTION()
	void HandleDepartmentChanged(FDepartmentId InDepartmentId);

	UFUNCTION()
	void HandlePopulationsSetup();

	UFUNCTION()
	void HandlePopulationsGrown();

	UFUNCTION()
	void HandlePopChanged(FPopId InPopId);

	UFUNCTION()
	void HandleNationDestroyed(FNationId InNationId);

	void BindFacades();
	void UnbindFacades();
	void RefreshMapModeEntries();

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
	TObjectPtr<UNations> Nations;

	UPROPERTY(Transient)
	TObjectPtr<UOrganizations> Organizations;

	UPROPERTY(Transient)
	TObjectPtr<UDepartments> Departments;

	UPROPERTY(Transient)
	TObjectPtr<UBuildings> Buildings;

	UPROPERTY(Transient)
	TObjectPtr<UPopulations> Populations;

	UPROPERTY(Transient)
	TObjectPtr<UBanking> Banking;

	UPROPERTY(Transient)
	TObjectPtr<UCharacters> Characters;

	UPROPERTY(Transient)
	TObjectPtr<URoles> Roles;

	UPROPERTY()
	TObjectPtr<ARtsPlayer> Player;

	/** The pawn OnRightMouseButtonClicked is bound to, tracked separately from Player because the
	 * pawn can change (or be unpossessed) without the controlled character changing. */
	UPROPERTY()
	TObjectPtr<ARtsMapPawn> Pawn;

	/** The pick itself. A plain UPROPERTY rather than FieldNotify: nothing binds to the asset, only
	 * to SelectedBuildingKind and SelectedBuildingCost derived from it. */
	UPROPERTY()
	TObjectPtr<UBuildingType> SelectedBuildingType;


public:
	FCharacterId GetControlledCharacterId() const { return ControlledCharacterId; }
	EPlayerModal GetActiveModal() const { return ActiveModal; }
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
	EMapMode GetActiveMapMode() const { return ActiveMapMode; }
	FProvinceId GetSelectedProvinceId() const { return SelectedProvinceId; }
	int32 GetSelectedProvinceIdValue() const { return SelectedProvinceIdValue; }
	FProvinceId GetHoveredProvinceId() const { return HoveredProvinceId; }
	FText GetSelectedProvinceName() const { return SelectedProvinceName; }
	const TArray<TObjectPtr<UMapModeEntryViewModel>>& GetMapModes() const { return MapModes; }
	bool IsBuildMenuOpen() const { return bIsBuildMenuOpen; }
	int32 GetSelectedBuildingCost() const { return SelectedBuildingCost; }
	FText GetSelectedBuildingName() const { return SelectedBuildingName; }
	FText GetPendingOrganizationName() const { return PendingOrganizationName; }
};
