#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "BuildMenuWidgetViewModel.generated.h"

class UBuildingCategoryViewModel;

UCLASS(BlueprintType, meta=(MVVMAllowedContextCreationType="CreateInstance"))
class RTSUI_API UBuildMenuWidgetViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Pushed from URtsHUDViewModel::bIsBuildMenuOpen. Whether the menu is up is the HUD's state:
	 * a process opens it, the menu does not open itself. */
	UFUNCTION(BlueprintCallable, Category="Build Menu")
	void SetOpen(bool bInOpen);

	/** Pushed from URtsHUDViewModel::BuildingCategories — the sections, already carrying their rows
	 * with current cost, affordability and selection. */
	UFUNCTION(BlueprintCallable, Category="Build Menu")
	void SetCategories(const TArray<UBuildingCategoryViewModel*>& InCategories);

	UFUNCTION(BlueprintCallable, Category="Build Menu")
	void SetCategoryCount(int32 InCategoryCount);

	/** Pushed from URtsHUDViewModel::SelectedBuildingName, for a summary line outside the list. The
	 * rows carry their own bIsSelected, so the list itself needs neither of these. */
	UFUNCTION(BlueprintCallable, Category="Build Menu")
	void SetSelectedBuildingName(const FText& InSelectedBuildingName);

	UFUNCTION(BlueprintCallable, Category="Build Menu")
	void SetSelectedBuildingCost(int32 InSelectedBuildingCost);

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter=IsOpen, Category="Build Menu", meta=(AllowPrivateAccess))
	bool bIsOpen = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Build Menu", meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UBuildingCategoryViewModel>> Categories;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Build Menu", meta=(AllowPrivateAccess))
	int32 CategoryCount = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Build Menu", meta=(AllowPrivateAccess))
	FText SelectedBuildingName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Category="Build Menu", meta=(AllowPrivateAccess))
	int32 SelectedBuildingCost = 0;

public:
	bool IsOpen() const { return bIsOpen; }
	const TArray<TObjectPtr<UBuildingCategoryViewModel>>& GetCategories() const { return Categories; }
	int32 GetCategoryCount() const { return CategoryCount; }
	FText GetSelectedBuildingName() const { return SelectedBuildingName; }
	int32 GetSelectedBuildingCost() const { return SelectedBuildingCost; }
};
