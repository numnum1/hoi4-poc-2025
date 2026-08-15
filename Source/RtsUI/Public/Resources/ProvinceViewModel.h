#pragma once

#include "CoreMinimal.h"
#include "ProvinceInfo.h"
#include "RtsIds.h"
#include "MVVMViewModelBase.h"
#include "ProvinceViewModel.generated.h"

class UBuildings;
class UCharacters;
class UDepartments;
class UEcology;
class UNations;
class UOrganizations;
class UPopulations;
class UProvinces;
class USelectableOrganizationViewModel;
class USoundBase;

/** Everything the UI shows about one ethnicity, resolved from the library so a widget never has to
 * ask UPopulations for a name or color itself. */
USTRUCT(BlueprintType)
struct RTSUI_API FEthnicityDisplayData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Ethnicity")
	FEthnicityId EthnicityId;

	UPROPERTY(BlueprintReadOnly, Category="Ethnicity")
	FText Name;

	UPROPERTY(BlueprintReadOnly, Category="Ethnicity")
	FColor MapColor = FColor::White;

	// FText has no operator==, hence EqualTo. Needed because UE_MVVM_SET_PROPERTY_VALUE compares the
	// whole array before deciding to broadcast.
	bool operator==(const FEthnicityDisplayData& InOther) const
	{
		return EthnicityId == InOther.EthnicityId && Name.EqualTo(InOther.Name) && MapColor == InOther.MapColor;
	}
};

/** One ethnicity's share of a province's people. */
USTRUCT(BlueprintType)
struct RTSUI_API FPopulationInProvince
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Population")
	FEthnicityDisplayData Ethnicity;

	UPROPERTY(BlueprintReadOnly, Category="Population")
	int32 Amount = 0;

	bool operator==(const FPopulationInProvince& InOther) const
	{
		return Ethnicity == InOther.Ethnicity && Amount == InOther.Amount;
	}
};

/**
 * A read-only C++ MVVM view model for a single province, addressed by the same FProvinceId
 * every other province API uses. See UCharacterViewModel for the pattern this follows:
 * Initialize(Id) takes a single parameter so it stays usable as an MVVM function binding target,
 * resolves UProvinces (and UNations, for the owner's name) off the game state, binds
 * directly to UProvinces' events (filtered to this id), and refreshes flat FieldNotify
 * properties on every change.
 *
 * FProvinceDisplayInfo is taken apart into one property per member — UMG binds single values,
 * not structs.
 */
UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UProvinceViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Province Viewmodel")
	void Initialize(FProvinceId InProvinceId);

protected:
	//~ Identity ---------------------------------------------------------------------------------

	/** False once the province's entity has been destroyed. Display keeps its last known values. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsValid, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	FProvinceId ProvinceId;

	/** ProvinceId as a plain int32, for widgets that cannot bind the id struct. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	int32 ProvinceIdValue = INDEX_NONE;

	//~ FProvinceDisplayInfo ---------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	FName MapId = NAME_None;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	FText ProvinceName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	FSlateBrush Picture;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	TObjectPtr<USoundBase> InspectionSound;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Display", meta=(AllowPrivateAccess))
	TObjectPtr<UEcology> Ecology;

	//~ Entity state -----------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	FNationId OwningNationId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	int32 OwningNationIdValue = INDEX_NONE;

	/** Read through UNations directly — Provinces itself is not allowed to know that module. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	FText OwningNationName;

	/** Read through UNations directly — Provinces itself is not allowed to know that module. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	FSlateBrush OwningNationFlag;

	/** Hours offset from game time, read from FProvinceTimeFragment. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel", meta=(AllowPrivateAccess))
	int32 TimezoneOffset = 0;

	//~ Who is here --------------------------------------------------------------------------------
	// Derived through Departments/Buildings — Provinces itself is not allowed to know those modules.

	/** One row per organization with at least one department seated in this province — see
	 * URtsPresence::CollectOrganizationsIn. Display only from this view model's side: what a click on
	 * a row means (opening the organization screen) is UI-invented state this module is not allowed to
	 * know, so the row widget reports the click to UProvinceDetailWidget::ViewOrganization in RtsUI
	 * instead of this class re-broadcasting it. See "Callbacks gibt es nur auf Manual-ViewModels". */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Presence", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<USelectableOrganizationViewModel>> OrganizationsPresent;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Presence", meta=(AllowPrivateAccess))
	int32 OrganizationsPresentCount = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Presence", meta=(AllowPrivateAccess))
	int32 BuildingCount = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Presence", meta=(AllowPrivateAccess))
	int32 CharacterCount = 0;

	//~ Population ---------------------------------------------------------------------------------
	// Read through UPopulations directly — Provinces itself is not allowed to know that module.

	/** Who lives here, one entry per ethnicity, largest first. Plain values rather than a view model
	 * per ethnicity: this is a display list, not a set of entities. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Population", meta=(AllowPrivateAccess))
	TArray<FPopulationInProvince> Population;

	/** Everyone in the province, across all ethnicities. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Population", meta=(AllowPrivateAccess))
	int32 TotalPopulation = 0;

	/** The ethnicity most people here belong to, for a widget that shows one label instead of the
	 * whole list. Empty name and white if nobody lives here. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Province Viewmodel|Population", meta=(AllowPrivateAccess))
	FEthnicityDisplayData DominantEthnicity;

public:
	bool IsValid() const { return bIsValid; }
	FProvinceId GetProvinceId() const { return ProvinceId; }
	int32 GetProvinceIdValue() const { return ProvinceIdValue; }

	FName GetMapId() const { return MapId; }
	FText GetProvinceName() const { return ProvinceName; }
	FSlateBrush GetPicture() const { return Picture; }
	USoundBase* GetInspectionSound() const { return InspectionSound; }
	UEcology* GetEcology() const { return Ecology; }

	FNationId GetOwningNationId() const { return OwningNationId; }
	int32 GetOwningNationIdValue() const { return OwningNationIdValue; }
	FText GetOwningNationName() const { return OwningNationName; }
	FSlateBrush GetOwningNationFlag() const { return OwningNationFlag; }
	int32 GetTimezoneOffset() const { return TimezoneOffset; }

	const TArray<TObjectPtr<USelectableOrganizationViewModel>>& GetOrganizationsPresent() const { return OrganizationsPresent; }
	int32 GetOrganizationsPresentCount() const { return OrganizationsPresentCount; }
	int32 GetBuildingCount() const { return BuildingCount; }
	int32 GetCharacterCount() const { return CharacterCount; }

	const TArray<FPopulationInProvince>& GetPopulation() const { return Population; }
	int32 GetTotalPopulation() const { return TotalPopulation; }
	FEthnicityDisplayData GetDominantEthnicity() const { return DominantEthnicity; }

	/** How many people of one ethnicity live here, 0 if none do. */
	UFUNCTION(BlueprintPure, Category="Province Viewmodel|Population")
	int32 GetPopulationOfEthnicity(FEthnicityId InEthnicityId) const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Province Viewmodel")
	void Refresh();

private:
	//~ Begin UObject interface
	virtual void BeginDestroy() override;
	//~ End UObject interface

	void Unbind();

	UFUNCTION()
	void HandleProvinceEvent(FProvinceId InProvinceId);

	/** Bound to Departments'/Buildings' events unfiltered — none of them carry a province id, so
	 * there is no cheaper check than re-deriving presence, same as UCharacters::OnCurrentProvinceChanged
	 * below. */
	UFUNCTION()
	void HandlePresenceEvent(FDepartmentId InDepartmentId);

	UFUNCTION()
	void HandleBuildingEvent(FBuildingId InBuildingId);

	UFUNCTION()
	void HandleCharacterMoved(FCharacterId InCharacterId);

	/** Bound to OnPopulationsSetup/OnPopulationsGrown — neither carries a province id, both mean
	 * "something in this province may have changed". */
	UFUNCTION()
	void HandlePopulationsChanged();

	/** Bound to OnPopSizeChanged/OnPopDestroyed. Both carry an FPopId, not a province id — by the
	 * time OnPopDestroyed fires the pop's fragments are already gone, so there is no cheaper check
	 * than re-deriving Population, same reasoning as HandlePresenceEvent above. */
	UFUNCTION()
	void HandlePopEvent(FPopId InPopId);

	/** Rebuilds OrganizationsPresent from URtsPresence::CollectOrganizationsIn. */
	void RefreshOrganizationsPresent();

	/** Rebuilds Population from UPopulations::CollectPopsInProvince. */
	void RefreshPopulation();

	UPROPERTY()
	TObjectPtr<UProvinces> Provinces;

	UPROPERTY()
	TObjectPtr<UNations> Nations;

	UPROPERTY()
	TObjectPtr<UOrganizations> Organizations;

	UPROPERTY()
	TObjectPtr<UDepartments> Departments;

	UPROPERTY()
	TObjectPtr<UBuildings> Buildings;

	UPROPERTY()
	TObjectPtr<UCharacters> Characters;

	UPROPERTY()
	TObjectPtr<UPopulations> Populations;

	/** Reused across refreshes so a rebuild that offers the same organizations keeps the same row
	 * objects — same idea as UPlayerCharacterViewModel::ActionEntryCache. */
	UPROPERTY()
	TMap<FOrganizationId, TObjectPtr<USelectableOrganizationViewModel>> OrganizationRowCache;
};
