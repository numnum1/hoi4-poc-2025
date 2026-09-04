#pragma once

#include "CoreMinimal.h"
#include "MapModes/MapModeType.h"
#include "MapModes/RtsMapModeContext.h"
#include "MapTooltip/MapTooltipContent.h"
#include "MapTypes.h"
#include "MVVMViewModelBase.h"
#include "ProvinceVisuals.h"
#include "RtsIds.h"
#include "UseWorldMapView.generated.h"

class UBuildings;
class UBuildingType;
class UCharacters;
class UDepartments;
class UMapModeEntryViewModel;
class UNations;
class UOrganizations;
class UPopulations;
class UProvinces;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UUseWorldMapView : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize();

	UFUNCTION(BlueprintCallable, Category="Use World Map View")
	void SetActiveMapMode(EMapMode InActiveMapMode);

	UFUNCTION(BlueprintCallable, Category="Use World Map View")
	void SetHoveredProvinceId(FProvinceId InHoveredProvinceId);

	void SetFoundingSiteOverlay(FCharacterId InActorId, UBuildingType* InHeadquartersType);

	UFUNCTION(BlueprintPure, FieldNotify, Category="Use World Map View")
	TMap<FProvinceId, FProvinceVisuals> GetProvinceVisuals() const;

	UFUNCTION(BlueprintPure, FieldNotify, Category="Use World Map View")
	TMap<FNationId, FWorldMapNationData> GetNationData() const;

	UFUNCTION(BlueprintPure, FieldNotify, Category="Use World Map View")
	FMapTooltipContent ComputeTooltip() const;

	EMapMode GetActiveMapMode() const { return ActiveMapMode; }
	FProvinceId GetHoveredProvinceId() const { return HoveredProvinceId; }

protected:
	virtual void BeginDestroy() override;

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use World Map View", meta=(AllowPrivateAccess))
	EMapMode ActiveMapMode = EMapMode::EMM_Political;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use World Map View", meta=(AllowPrivateAccess))
	FProvinceId HoveredProvinceId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category="Use World Map View", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UMapModeEntryViewModel>> MapModes;

	UFUNCTION()
	void HandleMapModeClicked(UMapModeEntryViewModel* InClickedEntry);

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
	FRtsMapModeContext MakeContext() const;

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
	TObjectPtr<UCharacters> Characters;

	UPROPERTY()
	TObjectPtr<UBuildingType> FoundingSiteHeadquartersType;

	FCharacterId FoundingSiteActorId;
};
