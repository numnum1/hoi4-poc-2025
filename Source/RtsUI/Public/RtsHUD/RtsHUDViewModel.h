#pragma once

#include "CoreMinimal.h"
#include "MapModes/MapModeType.h"
#include "MVVMViewModelBase.h"
#include "PlayedCharacterView/PlayerCharacterViewModel.h"
#include "ProvinceVisuals.h"
#include "RtsHUDViewModel.generated.h"

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

	/** The color of every living province under the active map mode. Computed on demand — see the
	 * class comment for why this is not a stored property. */
	UFUNCTION(BlueprintPure, FieldNotify, Category = "Rts HUD ViewModel")
	TMap<FProvinceId, FProvinceVisuals> GetProvinceVisuals() const;

protected:
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

	void BindFacades();
	void UnbindFacades();
	void RefreshMapModeEntries();

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

public:
	FCharacterId GetControlledCharacterId() const { return ControlledCharacterId; }
	EPlayerModal GetActiveModal() const { return ActiveModal; }
	int32 GetSelectedPositionIndex() const { return SelectedPositionIndex; }
	EMapMode GetActiveMapMode() const { return ActiveMapMode; }
	const TArray<TObjectPtr<UMapModeEntryViewModel>>& GetMapModes() const { return MapModes; }
};
