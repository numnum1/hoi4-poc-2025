#pragma once

#include "CoreMinimal.h"
#include "MapModes/MapModeType.h"
#include "MapTypes.h"
#include "MVVMViewModelBase.h"
#include "PlayedCharacterView/PlayerCharacterViewModel.h"
#include "ProvinceVisuals.h"
#include "MapTooltip/MapTooltipContent.h"
#include "RtsHUDViewModel.generated.h"

class ARtsMapPawn;
class ARtsPlayer;
class UBuildings;
class UDepartments;
class UMapModeEntryViewModel;
class UNations;
class UOrganizations;
class UPopulations;
class UProvinces;

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
	void SetControlledCharacterId(FCharacterId InControlledCharacterId);

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

#pragma region Tooltip
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	FMapTooltipContent ComputeTooltip() const;
#pragma endregion 
	
	virtual void BeginDestroy() override;

private:
	// TODO: Make this computed
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

	/** One row per mode, cached rather than rebuilt: the set of modes is fixed, only which one is
	 * active changes. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category = "Rts HUD ViewModel", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<UMapModeEntryViewModel>> MapModes;

	UFUNCTION()
	void HandleMapModeClicked(UMapModeEntryViewModel* InClickedEntry);

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

	/** Resolves the local ARtsPlayer and binds HandleControlledCharacterChanged to it — called from
	 * Initialize(), not from SetControlledCharacterId, so the right-click binding does not depend on
	 * Blueprint ever pushing a controlled character id in. Idempotent, safe to call again. */
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

	UPROPERTY()
	TObjectPtr<ARtsPlayer> Player;

	/** The pawn OnRightMouseButtonClicked is bound to, tracked separately from Player because the
	 * pawn can change (or be unpossessed) without SetControlledCharacterId firing. */
	UPROPERTY()
	TObjectPtr<ARtsMapPawn> Pawn;

public:
	FCharacterId GetControlledCharacterId() const { return ControlledCharacterId; }
	EPlayerModal GetActiveModal() const { return ActiveModal; }
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
	EMapMode GetActiveMapMode() const { return ActiveMapMode; }
	FProvinceId GetSelectedProvinceId() const { return SelectedProvinceId; }
	int32 GetSelectedProvinceIdValue() const { return SelectedProvinceIdValue; }
	FProvinceId GetHoveredProvinceId() const { return HoveredProvinceId; }
	const TArray<TObjectPtr<UMapModeEntryViewModel>>& GetMapModes() const { return MapModes; }
};
